/**
 * @file       Pipeline_Main.cpp
 * @brief      Main source file for implementation of pipeline project simulation
 * @mainpage
 * @author     Mark L. Short,
 *
 * @date       Oct 31, 2014
 *
 * <b>Course:</b>     CMPS 5133 Advanced Computer Architecture
 *
 * <b>Instructor:</b> Dr. Nelson Passos
 *
 * <b>Assignment:</b>
 *
 *    Your just got hired by a company that produces processors. Their main goal is 
 *    to start using pipeline design in their processors, but they heard rumors that 
 *    data dependence may negatively affect the performance of such processors. Your 
 *    job is to verify that assertion and to show how a four stage pipeline (Fetch, 
 *    Decode, Execute, Write Back) works. Data fetching happens during the execution 
 *    stage. No branch instructions are considered so the code runs straight from 
 *    beginning to end according with the initial order of the instructions. Each 
 *    instruction stage consumes one cycle of the processor. Resulting data is available 
 *    only after the Write Back stage (no forward circuits or any other design optimization). 
 *    In order to perform your task you receive a sequence of instructions (first 
 *    line of data) and its perspective dependency graph. Your program should read 
 *    the data and present the overlapped execution of those instructions. The program 
 *    must be able to handle 25 instructions.
 *
 * <b>Cite:</b>
 *         * Modern Processor Design, John Paul Shen, Mikko H. Lipasti, 2005
 *         * Runtime Dependency Analysis for Loop Pipelining in High-Level Synthesis,
 *           Alle, Morvan, Derien, IRISA / University of Rennes
 *         * Characterization of Branch and Data Dependencies in Programs for Evaluating 
 *           Pipeline Performance, Emma, Davidson, IEEE Transactions on Computer, 1987
 *         * Instruction Scheduling, Cambridge University UK, 2005, 
 *           http://www.cl.cam.ac.uk/teaching/2005/OptComp/slides/lecture14.pdf
 *         * The Optimum Pipeline Depth for a Microprocessor, IBM, 2005
 *         * Graph Partitioning Implementation Strategy, University of CA, Berkeley,
 *           http://parlab.eecs.berkeley.edu/wiki/_media/patterns/graph_partitioning.pdf
 *         * Data Structures and Algorithms with Object-Oriented Design Patterns in C++,
 *           Preiss, 1997, http://www.brpreiss.com/books/opus4/html/page9.html
 *         * Data Abstraction & Problem Solving with C++, Carrano, 2007,
 *           http://www.cs.rutgers.edu/~szhou/351/Graphs.pdf
 *         * Technical Report - Polymorphic C++ Debugging for System Design, Doucet, Gupta, 
 *           University of CA, Irvine, 2000, http://mesl.ucsd.edu/site/pubs/UCI-CECS-TR00-06.pdf
 *
 */

#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <locale>
#include <codecvt>
#include "DependencyGraph.h"
#include "PipelineSim.h"

#if defined(UNICODE) || defined(_UNICODE)
    #define tifstream      std::wifstream
    #define tstringstream  std::wstringstream
#else
    #define tifstream      std::ifstream
    #define tstringstream  std::stringstream
#endif

/// maximum instructions specified
constexpr int MAX_INSTRUCTIONS           = 25;
/// non-overlapped cycles required to execute 1 instruction in a 4 staged-pipeline
constexpr int BASE_CYCLES_PER_INSTUCTION = 4;

/// File used to read in test case data
constexpr TCHAR  g_szFileName[] = _T("InstructionInputData.txt");
/// Global directed acyclic graph object
CDependencyGraph g_DAG(MAX_INSTRUCTIONS);
/// Global pipeline simulation object
CPipelineSim     g_PipelineSim;


/**
 * @brief CalculateSequentialExecutionCycles calculates the number
 * of cycles required to "sequentially" execute a set of instructions. 
 *
 * The basic formula to calculate the execution cycles required to
 * run N instructions sequentially (non-overlapped) in this scenario
 * is: <b>N * 4 cycles</b>
 *
 * @param [in] dag          DAG object containing a list of instructions
 *
 * @retval int              the number sequential cycles required to run 
 *                          the instructions contained in DAG
 */
int  CalculateSequentialExecutionCycles(const CDependencyGraph& dag) noexcept;

/**
 * @brief CalculateCompleteOverlappedExecutionCycles calculates the best 
 * case execution scenario in terms of minimum number of cycles required
 * to execute the set of instructions.
 *
 * The basic formula to calculate the execution cycles required to run
 * N instructions in a 4 staged pipeline is something like:
 *
 * 1. Calculate number of cycles to execute 1st instruction, in this case
 *    it is 4 cycles.
 * 2. Then, based on the fact that a subsequent instruction will complete
 *    every cycle from cycle 4 on, for N number of instructions, therefore
 *    it will take <b>N + 3 cycles</b> to run the entire set of N instructions.
 *
 * @param [in] dag          DAG object containing a list of instructions
 *
 * @retval int              the number of overlapped cycles (with no delays) 
 *                          required to run the instructions contained in DAG
 */
int  CalculateCompleteOverlappedExecutionCycles ( const CDependencyGraph& dag ) noexcept;

/** 
* @brief CalculatePartialOverlappedExecutionCycles computes the number of
* cycles required to execute a set of instruction using a 4-staged pipeline
* and factoring in delays introduced to address instruction-level data
* dependencies.
*
* The basic formula needed to calculate the execution cycles required to
* run N instructions in a 4 staged pipeline is :
*
* 1. Use the calculation from the formula above to determine the minimum
*    number of cycles required. In this case it is: 
*    <b>N + 3 cycles</b>.
* 2. Then add 1 cycle for each delay introduced.
*
* Considering the initial program data provided in this assignment :
*
* 1. 6 instructions to be executed. Minimum execution time is N + 3 cycles,
*    or 9 cycles in this case.
* 2. 2 bubbles or stalls were introduced due to data - dependencies.
* 3. 9 cycles + 2 cycles ( for the stalls ) = 11 cycles. This is the same
*    result as reported in the assignment example.
* 4. So the resultant formula to calculate the number of cycles for a
*    partial overlapped pipelined execution of N instructions given M stalls
*    introduced is :
*    <b>N + 3 + M cycles</b>
*
*   So, how do we find the number of stalls required to be introduced due
*   to data dependencies ? In this scenario given only a 4 cycle "data
*   hazard" window of opportunity, and considering data reads occur in the
*   3rd stage ( EX ), while data writes are only accessible after WB ( to be
*   interpreted as the 5th stage ), that further narrows the "data hazard
*   window" down to 2 cycles.  The only way a data hazard could occur
*   is if there is a data dependency between two immediately sequential
*   instructions.
*
*   It is deduced that the very worst possible case of extreme data
*   dependency requiring a stall for every instruction would only require
*   at most :
*
*   <b>N + 3 cycles</b> for the instructions, plus another 
*   <b>N - 1 cycles</b> for adding 1 bubble / stall cycle for
*   every instruction after the 1st.
*
*   Therefore <b>( 2 * N ) + 2 cycles</b> would be the worst 
*   possible number of cycles required to run any set of instructions 
*   overlapped.
*
* @param [in] dag          DAG object containing a list of instructions
*
* @retval int              the number of overlapped cycles (with delays)
*                          required to run the instructions contained in DAG
*/
int CalculatePartialOverlappedExecutionCycles ( const CDependencyGraph& dag ) noexcept;

/**
 * @brief CalculateNumberOfStallsRequired calculates data-dependent 
 * pipeline stalls.
 *
 * This function identifies and calculates the number Pipeline stalls 
 * introduced into the pipeline to avoid data-dependency hazards in 
 * the pipelined execution of the set of instructions.
 *
 * @param [in] dag          DAG object containing a list of instructions
 *
 * @retval int              the number pipeline stalls required to  
 *                          address instruction data dependencies 
 *                          identified in a the DAGi
 */
int  CalculateNumberOfStallsRequired ( const CDependencyGraph& dag ) noexcept;

/**
 * @brief Performs basic pipeline process simulation.
 *
 * ExecutePipelineSimulation takes instruction data contain in an DAG
 * and feeds it to the simulation object for running of the instruction
 * pipeline simulation
 *
 * @param [in,out] sim      Simulation object
 * @param [in] dag          DAG object containing a list of instructions
 *
 * @retval true             on success
 * @retval false            on error
 *
 */
bool ExecutePipelineSimulation ( CPipelineSim& sim, const CDependencyGraph& dag ) noexcept;


/**
 * @brief LoadData performs basic file level data input.
 *
 * This method reads input data from text file and returns contents in a 
 * directed acyclic graph
 *
 * @param [in]  szFileName   name of the data file to be loaded
 * @param [out] dag          reference to a dag object
 *
 * @retval size_t       the number of item nodes read into the graph
 */
size_t LoadData ( const TCHAR* szFileName, CDependencyGraph& dag );


int _tmain ( int argc, _TCHAR* argv[] )
{
    if (LoadData(g_szFileName, g_DAG) == 0)
    {
        // try the Data directory next
        tstring strDataDir(_T("..\\Data\\"));
        strDataDir += g_szFileName;

        LoadData(strDataDir.c_str(), g_DAG);
    }

    ExecutePipelineSimulation(g_PipelineSim, g_DAG);

    TCHAR iAnyKey;
    tcout << _T("press (q) to quit ");
    tcin  >> iAnyKey;
    tcout << std::endl << _T("close enough!");

    return 0;
}

size_t LoadData ( const TCHAR* szFileName, CDependencyGraph& dag )
{
    size_t nReturn = 0;

    tifstream infile;

#if defined(UNICODE) || defined(_UNICODE)
    std::locale utf8_locale ( std::locale ( infile.getloc ( ) ), 
                              new std::codecvt_utf8_utf16<wchar_t> );
    infile.imbue ( utf8_locale );
#endif

    infile.open ( szFileName );

    if ( infile.bad ( ) )
    {
        tcout << _T ( "Error opening data file:" ) << szFileName << std::endl;
    }
    else
    {
        TCHAR szLineBuffer[128] = { 0 };

        // read in the 1st line of input, this will contain the list of instructions
        infile.getline ( szLineBuffer, _countof ( szLineBuffer ) - 1 );

        // parse the instruction list, removing trailing punctuation.
        tstringstream strStream;

        strStream << szLineBuffer;

        while ( strStream.getline ( szLineBuffer, 5, _T ( ' ' ) ) && 
              ( nReturn < MAX_INSTRUCTIONS ) )
        {
            dag.AddNode ( szLineBuffer[0] );

            nReturn++;
        }

        // now parse the instruction dependencies
        // this will be in the format of:
        //      B<space>A<NL>
        // where "B A" means that B depends on the result of A

        TCHAR idSrcNode = 0;
        TCHAR idDestNode = 0;

        while ( infile >> idSrcNode >> idDestNode )
        {
            // in estimating an edge weight, lets use the time delta or "dependency
            // distance" between when the 2 instructions are scheduled to begin execution.

            int iWeight = idSrcNode - idDestNode;

            dag.AddEdge ( idSrcNode, idDestNode, iWeight );
        }

        infile.close ( );
    }

    return nReturn;
}

int CalculateSequentialExecutionCycles ( const CDependencyGraph& dag ) noexcept
{
    return dag.GetNumNodes ( ) * BASE_CYCLES_PER_INSTUCTION;
}

int CalculateCompleteOverlappedExecutionCycles ( const CDependencyGraph& dag ) noexcept
{
    return dag.GetNumNodes ( ) + 3;
}

int CalculatePartialOverlappedExecutionCycles ( const CDependencyGraph& dag ) noexcept
{
    return dag.GetNumNodes() + CalculateNumberOfStallsRequired(dag) + 3;
}

int  CalculateNumberOfStallsRequired ( const CDependencyGraph& dag ) noexcept
{
    int iNumStalls = 0;

    CDependencyGraph::const_iterator it;

    for (it = dag.begin(); it != dag.end(); ++ it )
    {
        if (it->IsValid())
        {
            NODE_ID_T idNode = it->GetNodeID();
            // following will determine if an instruction is dependent on
            // an immediately previous one, (i.e B->A), which is the only
            // case that any stall is required to be introduced.
            if (it->HasEdge(idNode - 1))
                iNumStalls++;
        }
    }

    return iNumStalls;
}

bool ExecutePipelineSimulation ( CPipelineSim& sim, const CDependencyGraph& dag) noexcept
{
    bool bReturn = false;

    // add the loaded instructions to the pipeline simulator
    CDependencyGraph::const_iterator it;

    for ( it = dag.begin ( ); it != dag.end ( ); ++it )
    {
        if ( it->IsValid ( ) )
        {
            bool bDataDependent = false;
            NODE_ID_T idNode = it->GetNodeID ( );

            if ( it->HasEdge ( idNode - 1 ) )
                bDataDependent = true;

            sim.InsertInstruction(CInstructionData(idNode, bDataDependent) );
        }
    }


    tcout << _T ( "Total time for sequential (non overlapped) execution: " )
          << CalculateSequentialExecutionCycles ( dag ) << _T ( " cycles" ) << std::endl;
    tcout << _T ("------------------------------------------------------------------")
          << std::endl;
    tcout << _T ( "Overlapped execution:" ) << std::endl;

    bool bMoreInstructions = sim.ProcessNextCycle();

    while (bMoreInstructions)
    {
        sim.OutputCurrentInstructionCycle(tcout);

        bMoreInstructions = sim.ProcessNextCycle();
    }

    tcout << _T ( "------------------------------------------------------------------")
          << std::endl;
    tcout << _T ( "Total time for pipelined (overlapped) execution: " )
          << CalculatePartialOverlappedExecutionCycles ( dag ) << _T ( " cycles" ) << std::endl;

    return bReturn;
}