/*jshint globalstrict:false, strict:false, maxlen: 500 */
/*global assertEqual, assertNotEqual, assertTrue, AQL_EXPLAIN, AQL_EXECUTE */

////////////////////////////////////////////////////////////////////////////////
/// @brief tests for optimizer rules
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2012 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

const jsunity = require("jsunity");
const deriveTestSuite = require('@arangodb/test-helper').deriveTestSuite;
const base = require("fs").join(require('internal').pathForTesting('server'),
  'aql', 'aql-optimizer-rule-views-sorted-indexes.inc');
const AqlOptimizerRuleViewsSortedIndexesTestSuite = require("internal").load(base);

function optimizerRuleArangoSearchTestSuite() {
  let suite = {};
  deriveTestSuite(
    AqlOptimizerRuleViewsSortedIndexesTestSuite(false),
    suite,
    "_arangosearch"
  );
  return suite;
}

function optimizerRuleSearchAliasTestSuite() {
  let suite = {};
  deriveTestSuite(
    AqlOptimizerRuleViewsSortedIndexesTestSuite(true),
    suite,
    "_search-alias"
  );
  return suite;
}

jsunity.run(optimizerRuleArangoSearchTestSuite);
jsunity.run(optimizerRuleSearchAliasTestSuite);

return jsunity.done();
