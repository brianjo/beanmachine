/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <vector>

using uint = unsigned int;

namespace beanmachine::minibmg {

class Graph;
class Node;
class OperatorNode;
class ConstantNode;

enum Operator {
  // A scalar constant, like 1.2.
  // Result: the given constant value (REAL)
  CONSTANT,

  // Add two scalars.
  // Result: the sum (REAL)
  ADD,

  // Multiply two scalars.
  // Result: the product (REAL)
  MULTIPLY,

  // A normal distribution.
  // Parameters:
  // - mean (REAL)
  // - standard deviation (REAL)
  // Result: the distribution (DISTRIBUTION)
  DISTRIBUTION_NORMAL,

  // A beta distribution.
  // Parameters:
  // - ??? (REAL)
  // - ??? (REAL)
  // Result: the distribution.
  DISTRIBUTION_BETA,

  // A bernoulli distribution (DISTRIBUTION).
  // Parameters:
  // - probability of yeilding 1 (as opposed to 0) (REAL)
  // Result: the distribution (DISTRIBUTION)
  DISTRIBUTION_BERNOULLI,

  // Draw a sample from the distribution parameter.
  // Parameters:
  // - ditribution
  // Result: A sample from the distribution (REAL)
  SAMPLE,

  // Observe a sample from the distribution parameter.
  // Parameters:
  // - ditribution (DISTRIBUTION)
  // - value (REAL)
  // Result: NONE.
  OBSERVE,

  // Query an intermediate result in the graph.
  // Parameters:
  // - value (REAL)
  // Result: NONE.
  QUERY,

  // Not a real operator.  Used as a limit when looping through operators.
  LAST,
};

enum Type {
  // No type.  For example, the result of an observation or query node.
  NONE,
  // A scalar real value.
  REAL,
  // A distribution of real values.
  DISTRIBUTION,
};

class Graph {
 public:
  const std::vector<const Node*> nodes;

  // valudates that the list of nodes forms a valid graph,
  // and returns that graph.  Throws an exception if the
  // nodes do not form a valid graph.
  static Graph create(std::vector<const Node*> nodes);
  static void validate();
  ~Graph();

 private:
  // A private constructor that forms a graph without validation.
  // Used internally.  All exposed graphs should be validated.
  explicit Graph(std::vector<const Node*> nodes);

 public:
  class Factory {
   public:
    uint add_constant(double value);
    uint add_operator(enum Operator op, std::vector<uint> parents);
    uint add_query(uint parent); // returns query id
    const Node* get_node(uint node_id);
    Graph build();
    ~Factory();

   private:
    std::vector<const Node*> nodes;
    int next_query;
  };
};

class Node {
 public:
  Node(const uint sequence, const enum Operator op, const Type type)
      : sequence(sequence), op(op), type(type) {}
  const uint sequence;
  const enum Operator op;
  const Type type;
  virtual ~Node() {}
};

class OperatorNode : public Node {
 public:
  OperatorNode(
      const std::vector<const Node*>& in_nodes,
      const uint sequence,
      const enum Operator op,
      const Type type)
      : Node(sequence, op, type), in_nodes(in_nodes) {}
  const std::vector<const Node*> in_nodes;
};

class ConstantNode : public Node {
 public:
  ConstantNode(
      const double value,
      const uint sequence,
      const enum Operator op,
      const Type type)
      : Node(sequence, op, type), value(value) {}
  const double value;
};

class QueryNode : public OperatorNode {
 public:
  QueryNode(
      const uint query_index,
      const std::vector<const Node*>& in_nodes,
      const uint sequence,
      const enum Operator op,
      const Type type)
      : OperatorNode(in_nodes, sequence, op, type), query_index(query_index) {}
  const uint query_index;
};

} // namespace beanmachine::minibmg
