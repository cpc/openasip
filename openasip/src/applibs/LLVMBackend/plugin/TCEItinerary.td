//copied from:
//=-HexagonScheduleV4.td - HexagonV4 Scheduling Definitions --*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


// There are Six SLOTS (four parallel pipelines) in TTA
// This file describes that machine information.

// FU's supported by xxx
def FU_FU0   : FuncUnit;
def FU_FU1   : FuncUnit;
def FU_FU2   : FuncUnit;
def FU_FU3   : FuncUnit;


// Instruction Support
def IT_FU    : InstrItinClass;

// Itinerary class for a given chip set
def TCEItineraries : ProcessorItineraries<[FU_FU0, FU_FU1, FU_FU2, FU_FU3], [], [
  InstrItinData<IT_FU, [InstrStage<1, [FU_FU0, FU_FU1, FU_FU2, FU_FU3]>]>]>;

// Processor Model
def TCEModelV0 : SchedMachineModel {
  // Max issue per cycle == bundle width.
  let IssueWidth = 4;
  let Itineraries = TCEItineraries;
  let LoadLatency = 1;
  let CompleteModel = 0;
}
