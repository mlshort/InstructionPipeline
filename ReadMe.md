#Instruction Pipeline Simulation

DIRECTORY STRUCTURE: 
=======================================================================
\Instruction Pipeline
+ --- \PipelineProject  (source)
+ --- \Bin              (compiled executable)
+ --- \Data             (input file data)
+ --- \Documentation    (implementation documentation)
+ --- \Reference Documents
+ --- \Obj              (discardable binary intermediaries)
 
DATE WRITTEN:        November 25, 2014  

Instruction Pipeline Project Overview
========================================================================

1.   ASSIGNMENT:

     CMPS 5133 Advanced Computer Architecture:
 
     Your just got hired by a company that produces processors. Their main goal is 
     to start using pipeline design in their processors, but they heard rumors that 
     data dependence may negatively affect the performance of such processors. Your 
     job is to verify that assertion and to show how a four stage pipeline (Fetch, 
     Decode, Execute, Write Back) works. Data fetching happens during the execution 
     stage. No branch instructions are considered so the code runs straight from 
     beginning to end according with the initial order of the instructions. Each 
     instruction stage consumes one cycle of the processor. Resulting data is available 
     only after the Write Back stage (no forward circuits or any other design optimization). 
     In order to perform your task you receive a sequence of instructions (first 
     line of data) and its perspective dependency graph. Your program should read 
     the data and present the overlapped execution of those instructions. The program 
     must be able to handle 25 instructions.
     
Reference(s)
============================================================================
  * Modern Processor Design, John Paul Shen, Mikko H. Lipasti, 2005
  
  * Runtime Dependency Analysis for Loop Pipelining in High-Level Synthesis,
    Alle, Morvan, Derien, IRISA / University of Rennes
    
  * Characterization of Branch and Data Dependencies in Programs for Evaluating 
    Pipeline Performance, Emma, Davidson, IEEE Transactions on Computer, 1987
    
  * Instruction Scheduling, Cambridge University UK, 2005, 
    http://www.cl.cam.ac.uk/teaching/2005/OptComp/slides/lecture14.pdf
   
  * The Optimum Pipeline Depth for a Microprocessor, IBM, 2005
 
  * Graph Partitioning Implementation Strategy, University of CA, Berkeley,
    http://parlab.eecs.berkeley.edu/wiki/_media/patterns/graph_partitioning.pdf
   
  * Data Structures and Algorithms with Object-Oriented Design Patterns in C++,
    Preiss, 1997, http://www.brpreiss.com/books/opus4/html/page9.html
   
  * Data Abstraction & Problem Solving with C++, Carrano, 2007,
    http://www.cs.rutgers.edu/~szhou/351/Graphs.pdf
 
  * Technical Report - Polymorphic C++ Debugging for System Design, Doucet, Gupta, 
    University of CA, Irvine, 2000, http://mesl.ucsd.edu/site/pubs/UCI-CECS-TR00-06.pdf
      
