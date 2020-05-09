//=--ConstraintsGraph.cpp-----------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Implementation of methods in ConstraintsGraph.cpp
//
//===----------------------------------------------------------------------===//

#include "clang/CConv/ConstraintsGraph.h"
#include <iostream>
#include <boost/graph/graphviz.hpp>

// This has to be included to avoid linking errors with boost libraries.
#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>
#include <llvm/Support/raw_ostream.h>
void boost::throw_exception(std::exception const & e) {
//do nothing
}

ConstraintsGraph::vertex_t ConstraintsGraph::addVertex(Atom *A) {
  // Save all the const atoms.
  if (ConstAtom *CA = clang::dyn_cast<ConstAtom>(A)) {
    AllConstAtoms.insert(CA);
  }

  // If we haven't seen the Atom? Insert into the graph.
  if (AtomToVDMap.find(A) == AtomToVDMap.end()) {
    auto Vidx = add_vertex(A, CG);
    AtomToVDMap[A] = Vidx;
  }
  return AtomToVDMap[A];
}

std::set<ConstAtom*> &ConstraintsGraph::getAllConstAtoms() {
  return AllConstAtoms;
}

void ConstraintsGraph::addEdge(Atom *A1, Atom *A2, bool isBackward) {
  auto V1 = addVertex(A1);
  auto V2 = addVertex(A2);
  if (isBackward)
    add_edge(V2, V1, CG);
  else
    add_edge(V1, V2, CG);
}

void ConstraintsGraph::addConstraint(Geq *C, Constraints &CS) {
  Atom *A1 = C->getLHS();
  if (VarAtom *VA1 = clang::dyn_cast<VarAtom>(A1)) {
    A1 = CS.getOrCreateVar(VA1->getLoc());
  }
  Atom *A2 = C->getRHS();
  if (VarAtom *VA2 = clang::dyn_cast<VarAtom>(A2)) {
    A2 = CS.getOrCreateVar(VA2->getLoc());
  }
  // Here, LHS >= RHS, so edge is RHS -> LHS if checked; LHS -> RHS otherwise
  addEdge(A1,A2,C->constraintIsChecked());
}

void ConstraintsGraph::dumpCGDot(const std::string& GraphDotFile) {
   std::ofstream DotFile;
   DotFile.open(GraphDotFile);
   write_graphviz(DotFile, CG, [&] (std::ostream &out, unsigned v) {
     out << "[label=\"" << CG[v]->getStr() << "\"]";
   });
   DotFile.close();
}