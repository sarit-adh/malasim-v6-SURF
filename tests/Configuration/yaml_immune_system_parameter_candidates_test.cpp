#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include "Configuration/ImmuneSystemParameterCandidates.h"

class ImmuneSystemParameterCandidatesTest : public ::testing::Test {
protected:
  YAML::Node make_candidates_node(int used_in_simulation = 0, bool random_selection = false) {
    YAML::Node node;
    node["used_in_simulation"] = used_in_simulation;
    node["random_selection"] = random_selection;
    YAML::Node cmap;
    {
      YAML::Node c;
      c["p_ci_symp"]   = 0.5;
      c["z"]           = 3.0;
      c["kappa"]       = 0.1;
      c["midpoint"]    = 0.2;
      c["p_seek_base"] = 0.8;
      cmap[0] = c;
    }
    {
      YAML::Node c;
      c["p_ci_symp"]   = 0.6;
      c["z"]           = 3.0;
      c["kappa"]       = 0.1;
      c["midpoint"]    = 0.15;
      c["p_seek_base"] = 0.6;
      cmap[1] = c;
    }
    {
      YAML::Node c;
      c["p_ci_symp"]   = 1.0;
      c["z"]           = 1.5;
      c["kappa"]       = 0.5;
      c["midpoint"]    = 0.1;
      c["p_seek_base"] = 0.65;
      cmap[4] = c;
    }
    node["candidates"] = cmap;
    return node;
  }
};

// Decode a valid candidates block and check values
TEST_F(ImmuneSystemParameterCandidatesTest, DecodesValidCandidates) {
  auto node = make_candidates_node(1);
  ImmuneSystemParameterCandidates result;
  EXPECT_NO_THROW(YAML::convert<ImmuneSystemParameterCandidates>::decode(node, result));

  EXPECT_EQ(result.get_used_in_simulation(), 1);
  EXPECT_FALSE(result.get_random_selection());
  EXPECT_EQ(result.get_candidates().size(), 3u);

  const auto &c0 = result.get_candidates().at(0);
  EXPECT_DOUBLE_EQ(c0.p_ci_symp,   0.5);
  EXPECT_DOUBLE_EQ(c0.z,           3.0);
  EXPECT_DOUBLE_EQ(c0.kappa,       0.1);
  EXPECT_DOUBLE_EQ(c0.midpoint,    0.2);
  EXPECT_DOUBLE_EQ(c0.p_seek_base, 0.8);

  const auto &c1 = result.get_candidates().at(1);
  EXPECT_DOUBLE_EQ(c1.p_ci_symp,   0.6);
  EXPECT_DOUBLE_EQ(c1.p_seek_base, 0.6);

  const auto &c4 = result.get_candidates().at(4);
  EXPECT_DOUBLE_EQ(c4.p_ci_symp,   1.0);
  EXPECT_DOUBLE_EQ(c4.p_seek_base, 0.65);
}

// has_selected_candidate returns true for a present index
TEST_F(ImmuneSystemParameterCandidatesTest, HasSelectedCandidateTrue) {
  auto node = make_candidates_node(0);
  ImmuneSystemParameterCandidates result;
  YAML::convert<ImmuneSystemParameterCandidates>::decode(node, result);

  EXPECT_TRUE(result.has_selected_candidate());
  EXPECT_NO_THROW({
    const auto &c = result.get_selected_candidate();
    EXPECT_DOUBLE_EQ(c.z, 3.0);
  });
}

// has_selected_candidate returns false when used_in_simulation is missing
TEST_F(ImmuneSystemParameterCandidatesTest, HasSelectedCandidateFalseWhenMissing) {
  auto node = make_candidates_node(99);  // index 99 not in candidates
  ImmuneSystemParameterCandidates result;
  YAML::convert<ImmuneSystemParameterCandidates>::decode(node, result);

  EXPECT_FALSE(result.has_selected_candidate());
  EXPECT_THROW(result.get_selected_candidate(), std::runtime_error);
}

// Encode round-trips back to decodable YAML
TEST_F(ImmuneSystemParameterCandidatesTest, EncodeDecodeRoundtrip) {
  auto node = make_candidates_node(0);
  ImmuneSystemParameterCandidates original;
  YAML::convert<ImmuneSystemParameterCandidates>::decode(node, original);

  YAML::Node encoded = YAML::convert<ImmuneSystemParameterCandidates>::encode(original);

  ImmuneSystemParameterCandidates decoded;
  EXPECT_NO_THROW(YAML::convert<ImmuneSystemParameterCandidates>::decode(encoded, decoded));

  EXPECT_EQ(decoded.get_used_in_simulation(), 0);
  EXPECT_FALSE(decoded.get_random_selection());
  EXPECT_EQ(decoded.get_candidates().size(), original.get_candidates().size());
  EXPECT_DOUBLE_EQ(decoded.get_candidates().at(0).p_ci_symp,
                   original.get_candidates().at(0).p_ci_symp);
  EXPECT_DOUBLE_EQ(decoded.get_candidates().at(4).p_seek_base,
                   original.get_candidates().at(4).p_seek_base);
}

// random_selection is parsed and preserved by encode/decode
TEST_F(ImmuneSystemParameterCandidatesTest, RandomSelectionRoundtrip) {
  auto node = make_candidates_node(0, true);
  ImmuneSystemParameterCandidates original;
  YAML::convert<ImmuneSystemParameterCandidates>::decode(node, original);

  EXPECT_TRUE(original.get_random_selection());

  YAML::Node encoded = YAML::convert<ImmuneSystemParameterCandidates>::encode(original);

  ImmuneSystemParameterCandidates decoded;
  EXPECT_NO_THROW(YAML::convert<ImmuneSystemParameterCandidates>::decode(encoded, decoded));
  EXPECT_TRUE(decoded.get_random_selection());
}

// random_selection defaults to false when omitted
TEST_F(ImmuneSystemParameterCandidatesTest, RandomSelectionDefaultsToFalseWhenMissing) {
  auto node = make_candidates_node(0);
  node.remove("random_selection");

  ImmuneSystemParameterCandidates decoded;
  EXPECT_NO_THROW(YAML::convert<ImmuneSystemParameterCandidates>::decode(node, decoded));
  EXPECT_FALSE(decoded.get_random_selection());
}

// Missing 'used_in_simulation' throws
TEST_F(ImmuneSystemParameterCandidatesTest, MissingUsedInSimulationThrows) {
  YAML::Node node;
  node["candidates"] = YAML::Node(YAML::NodeType::Map);
  ImmuneSystemParameterCandidates result;
  EXPECT_THROW(YAML::convert<ImmuneSystemParameterCandidates>::decode(node, result),
               std::runtime_error);
}

// Missing 'candidates' throws
TEST_F(ImmuneSystemParameterCandidatesTest, MissingCandidatesThrows) {
  YAML::Node node;
  node["used_in_simulation"] = 0;
  ImmuneSystemParameterCandidates result;
  EXPECT_THROW(YAML::convert<ImmuneSystemParameterCandidates>::decode(node, result),
               std::runtime_error);
}

// A candidate with a missing field throws
TEST_F(ImmuneSystemParameterCandidatesTest, CandidateMissingFieldThrows) {
  YAML::Node node;
  node["used_in_simulation"] = 0;
  YAML::Node cmap;
  YAML::Node c;
  c["p_ci_symp"] = 0.5;
  // z, kappa, midpoint, p_seek_base intentionally omitted
  cmap[0] = c;
  node["candidates"] = cmap;

  ImmuneSystemParameterCandidates result;
  EXPECT_THROW(YAML::convert<ImmuneSystemParameterCandidates>::decode(node, result),
               std::runtime_error);
}

// log_all does not crash (smoke test)
TEST_F(ImmuneSystemParameterCandidatesTest, LogAllDoesNotCrash) {
  auto node = make_candidates_node(0);
  ImmuneSystemParameterCandidates result;
  YAML::convert<ImmuneSystemParameterCandidates>::decode(node, result);
  EXPECT_NO_THROW(result.log_all());
}
