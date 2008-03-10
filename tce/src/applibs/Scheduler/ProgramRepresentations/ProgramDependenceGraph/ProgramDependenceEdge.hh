/**
 * @file ProgramDependenceEdge.hh
 *
 * Declaration of prototype of graph-based program representation:
 * declaration of the program dependence edge.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_DEPENDENCE_EDGE_HH
#define TTA_PROGRAM_DEPENDENCE_EDGE_HH

#include "GraphEdge.hh"
#include "ControlDependenceEdge.hh"
#include "DataDependenceEdge.hh"

class ProgramDependenceEdge : public GraphEdge {
public:
    ProgramDependenceEdge(
        ControlDependenceEdge& cEdge,
        int edgeID);
    ProgramDependenceEdge(
        DataDependenceEdge& cEdge,
        int edgeID);

    virtual ~ProgramDependenceEdge();

    bool isControlDependence() const;
    bool isDataDependence() const ;
    ControlDependenceEdge& controlDependenceEdge();
    DataDependenceEdge& dataDependenceEdge();
    std::string toString() const;
    std::string dotString() const;
    
private:
    ControlDependenceEdge* cEdge_;
    DataDependenceEdge* dEdge_;
};

#endif
