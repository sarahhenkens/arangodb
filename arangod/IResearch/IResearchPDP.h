////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2022 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
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
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.hpp"
#include "Basics/Result.h"

namespace iresearch {
class parametric_description;
}  // namespace iresearch

namespace arangodb {
namespace iresearch {

constexpr ::iresearch::byte_type kMaxLevenshteinDistance = 4;
constexpr ::iresearch::byte_type kMaxDamerauLevenshteinDistance = 3;

const ::iresearch::parametric_description& getParametricDescription(
    ::iresearch::byte_type max_distance, bool with_transpositions);

}  // namespace iresearch
}  // namespace arangodb
