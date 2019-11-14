/**
* @file       PipelineSim.h
* @brief      CPipelineSim class interface
*
*
* @author     Mark L. Short
* @date       November 24, 2014
*/

#pragma once

#if !defined(_PIPELINE_SIM_H__)
#define _PIPELINE_SIM_H__

#ifndef _COMMON_DEF_H__
    #include "CommonDef.h"
#endif

#ifndef _INC_TCHAR
    #include <tchar.h>
#endif

#ifndef _QUEUE_
    #include <queue>
#endif

#ifndef _LIST_
    #include <list>
#endif

#ifndef _OSTREAM_
    #include <ostream>
#endif

#if defined(UNICODE) || defined(_UNICODE)
    #define tostream   std::wostream
#else
    #define tostream   std::ostream
#endif

/**
    @brief Pipeline Instruction State
*/
typedef enum PS_PIPELINE_STATE
{
    PS_INVALID,   ///< initial default state
    PS_IF,        ///< Instruction Fetch
    PS_ID,        ///< Instruction Decode
    PS_EX,        ///< Execute
    PS_WB,        ///< Write Back
    PS_COMPLETED  ///< instruction processing completed
} PS_PIPELINE_STATE_T;


/**
    In a more sophisticated simulation, the following would contain the
    actual instruction to be processed (either as a string or binary opcode);
    however, in this instance it is only a single letter ('a'..'y').
*/
typedef TCHAR INSTRUCTION_T;

/// used to denote an uninitialized instruction
constexpr INSTRUCTION_T INVALID_INSTRUCTION = 0;
/// used to denote a no-operation instruction
constexpr INSTRUCTION_T NOOP_INSTRUCTION    = '-';

/** 
    @brief Instruction data and state
*/
class CInstructionData
{
    INSTRUCTION_T     m_Instruction;
    PS_PIPELINE_STATE m_psState;
    bool              m_bDataDependent;
public:
    /// Default Constructor
    constexpr CInstructionData() noexcept
        : m_Instruction    ( INVALID_INSTRUCTION ),
          m_psState        ( PS_INVALID ),
          m_bDataDependent ( false )
    { };

    /// Initialization Constructor
    constexpr explicit CInstructionData ( const INSTRUCTION_T& instruction, 
                                          bool bDataDependent = false ) noexcept
        : m_Instruction    ( instruction ),
          m_psState        ( PS_INVALID ),
          m_bDataDependent ( bDataDependent )
    { };

    /// Initialization Constructor
    constexpr explicit CInstructionData ( const INSTRUCTION_T& instruction, 
                                          PS_PIPELINE_STATE psState, 
                                          bool bDataDependent = false ) noexcept
        : m_Instruction   ( instruction ),
          m_psState       ( psState ),
          m_bDataDependent( bDataDependent )
    { };

    /// Destructor
    ~CInstructionData() = default;

/**
    @brief  Retrieves the instruction

    @retval INSTRUCTION_T containing instruction type
*/
    constexpr INSTRUCTION_T  GetInstruction(void) const noexcept
    { return m_Instruction; };

/**
    @brief Gets the instruction pipeline state

    @retval PS_INVALID      initial default state
    @retval PS_IF           Instruction Fetch
    @retval PS_ID           Instruction Decode
    @retval PS_EX           Execute
    @retval PS_WB           Write Back
    @retval PS_COMPLETED    instruction processing completed
*/
    constexpr PS_PIPELINE_STATE GetState(void) const noexcept
    { return m_psState; };

/**
    @brief Sets the instruction pipeline state
*/
    void              SetState(PS_PIPELINE_STATE psSet) noexcept
    { m_psState = psSet; };

/**
    @retval true    if this instruction has been flagged
                    as data-dependent on another
    @retval false   if no data-dependencies have been
                    annotated
*/
    constexpr bool    IsDataDependent(void) const noexcept
    { return m_bDataDependent; };

    void              SetDataDependent(bool bSet = true) noexcept
    { m_bDataDependent = bSet; };

    constexpr bool    IsNOOP(void) const noexcept
    { return m_Instruction == NOOP_INSTRUCTION; };
};

/** 
    @brief simulates injecting a bubble into the pipeline
*/
class CNoopInstruction : public CInstructionData
{
public:
    /// Default Constructor
    constexpr CNoopInstruction() noexcept
        : CInstructionData(NOOP_INSTRUCTION)
    { };

    /// Initialization Constructor
    constexpr explicit CNoopInstruction(PS_PIPELINE_STATE psState) noexcept
        : CInstructionData(NOOP_INSTRUCTION, psState)
    { };
};

/**
    @brief A 4-staged pipeline simulation class
    
    The following class attempts to simulate the processing
    of instructions in a four-staged pipeline.  In a four-stage
    pipeline it is possible to execute 'sub-instructions' of 
    four separate instructions at the same time, with each one
    having a pipeline 'state' denoting what stage of the execution
    process it is in.  Additionally, no two instructions may share 
    the same 'state' concurrently.

    This class simulates a pipeline in the form of a linked-list
    to model the concurrent instruction processing.
*/
class CPipelineSim
{
    DWORD                        m_dwCycle;                ///< maintains current pipeline cycle
    DWORD                        m_dwStallCtr;             ///< a count of the stalls introduced
    DWORD                        m_dwCompletedCtr;         ///< count of instructions that completed execution
    DWORD                        m_dwMaxPipelineDepth;     ///< limit on instructions in the pipeline
    std::list<CInstructionData>  m_lstInstructionPipeline; ///< our instruction pipeline
    std::queue<CInstructionData> m_queInstructions;        ///< our instruction queue

public:
    /// Default Constructor
    CPipelineSim() noexcept;
    /// Default Destructor
    ~CPipelineSim() = default;
/**
    @brief Retrieves the current number of cycles executed

    @retval DWORD       current cycle
*/
    constexpr DWORD GetCycle(void) const noexcept
    { return m_dwCycle; };

/**
    @brief Retrieves the count of stalls introduced into the pipeline

    @retval DWORD   count of stalls
*/
    constexpr DWORD GetStallCount(void) const noexcept
    { return m_dwStallCtr; };

/**
    @brief Retrieves the current count of completed instructions

    @retval DWORD   count of completed instructions
*/
    constexpr DWORD GetCompletionCount(void) const noexcept
    { return m_dwCompletedCtr; };

/**
    @brief Process next pipeline instruction cycle

    Increments cycle counter and continues processing
    of the currently queued instructions, advancing each one
    to the next pipeline state accordingly.

    @retval true    if there are subsequent instructions to be
                    executed.
    @retval false   if there are no more instructions to be 
                    executed.
*/
    bool ProcessNextCycle(void) noexcept;

/**
    @brief Adds the instruction to the instruction queue.
        
    Queued instructions are popped off the queue and inserted into the 
    pipeline during the ProcessNextCycle method call. Instruction state is 
    updated accordingly to denote the current pipeline stage it is in.

    @param [in] instruction     data to add to the queue for
                                further insertion and processing in
                                the pipeline.

    @retval size_t              number of instructions in the queue
*/
    size_t InsertInstruction( const CInstructionData& instruction ) noexcept;

/**
    @brief formats and outputs current pipelined instructions to the provided stream

    @param [in,out] os          destination output stream

    @retval tostream&           reference to updated stream
*/
    tostream&   OutputCurrentInstructionCycle( tostream& os ) noexcept;


};

#endif
