/**
* @file       PipelineSim.cpp
* @brief      CPipelineSim class implementation
*
*
* @author     Mark L. Short
* @date       November 23, 2014
*/

#include "stdafx.h"
#include "PipelineSim.h"

/**
  four-stage pipeline only allows concurrent processing
  of four instructions at a time.
*/
constexpr int CONCURRENT_INSTRUCTION_LIMIT = 4;

typedef std::list<CInstructionData>::iterator         LstIterator;
typedef std::list<CInstructionData>::reverse_iterator rLstIterator;


CPipelineSim::CPipelineSim ( ) noexcept
    : m_dwCycle(0),
      m_dwStallCtr(0),
      m_dwCompletedCtr(0),
      m_dwMaxPipelineDepth ( CONCURRENT_INSTRUCTION_LIMIT ),
      m_lstInstructionPipeline(),
      m_queInstructions()
{
}

bool CPipelineSim::ProcessNextCycle(void) noexcept
{
    bool bReturn = false;
    // increment the cycle counter
    m_dwCycle++;

    // Begin processing our instruction queue
    // check our current instruction pipeline size and see if we have room
    if ( m_lstInstructionPipeline.size ( ) <= m_dwMaxPipelineDepth )
    {
        // check our instruction queue and see if we have anything left to execute

        if (m_queInstructions.size() != 0)
        {
            CInstructionData instruction = m_queInstructions.front();

            m_queInstructions.pop();

        // insert the instruction at the beginning of our pipeline
            m_lstInstructionPipeline.push_front ( instruction );

            bReturn = true;
        }
        else
        {
            // nothing left in the instruction que,
            // so we insert NOOPS until everything
            // clears the pipeline
            CNoopInstruction NOOP;

            m_lstInstructionPipeline.push_front ( NOOP );
        }
    }

    bool bStalled = false;
    // reverse iterate over the instruction currently in the pipeline 
    for (rLstIterator itr = m_lstInstructionPipeline.rbegin(); 
         itr != m_lstInstructionPipeline.rend() && (bStalled == false); ++itr)
    {
        PS_PIPELINE_STATE stInstruction = itr->GetState();

        switch (stInstruction) 
        {

            case PS_INVALID:   // initial default state
                itr->SetState(PS_IF);
                if ( itr->IsNOOP ( ) == false )
                    bReturn = true;
                break;

            case PS_IF:        // Instruction Fetch state
                itr->SetState(PS_ID);
                if ( itr->IsNOOP ( ) == false )
                    bReturn = true;
                break;

            case PS_ID:        // Instruction Decode state
                // need to verify if a dependency exists between this instruction
                // and the immediately previous instruction

                if (itr->IsDataDependent())
                {
                    // we have to introduce a stall here
                    itr->SetDataDependent(false);
                    
                    CNoopInstruction NOOP(PS_EX);
                    
                    m_lstInstructionPipeline.insert(itr.base(), NOOP);

                    bStalled = true;
                    m_dwStallCtr++;
                }
                else
                {
                    itr->SetState(PS_EX);
                }

                if (itr->IsNOOP() == false)
                    bReturn = true;
                break;

            case PS_EX:        // Instruction Execute state
                itr->SetState(PS_WB);
                if (itr->IsNOOP() == false)
                    bReturn = true;
                break;

            case PS_WB:         // Instruction Write Back state
                itr->SetState (PS_COMPLETED); // mark this for removal later

                if (itr->IsNOOP() == false)
                    m_dwCompletedCtr++;

                break;

            case PS_COMPLETED:
            default:

                break;
        }
    }

// check to see if we have a completed instruction for removal from the pipeline
    if (m_lstInstructionPipeline.back().GetState() == PS_COMPLETED)
        m_lstInstructionPipeline.pop_back();

    return bReturn;
};

size_t CPipelineSim::InsertInstruction ( const CInstructionData& instruction ) noexcept
{
    m_queInstructions.push(instruction);

    return m_queInstructions.size();
};

tostream& CPipelineSim::OutputCurrentInstructionCycle ( tostream& os ) noexcept
{
    for ( LstIterator it = m_lstInstructionPipeline.begin ( ); 
          it != m_lstInstructionPipeline.end ( ); ++it )
    {
        switch (it->GetState())
        {
            case PS_IF:
            case PS_ID:
            case PS_EX:
            case PS_WB:
                os << it->GetInstruction() << _T(" ");
                break;
            default:
                break;
        }
    }

    os << std::endl;

    return os;
}

