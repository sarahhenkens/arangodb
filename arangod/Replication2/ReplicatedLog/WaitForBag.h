////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2022-2022 ArangoDB GmbH, Cologne, Germany
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
/// @author Tobias Gödderz
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <exception>
#include <vector>

namespace arangodb::futures {
template<typename T>
class Future;
template<typename T>
class Promise;
struct Unit;
}  // namespace arangodb::futures

namespace arangodb {

struct WaitForBag {
  WaitForBag() = default;

  auto addWaitFor() -> futures::Future<futures::Unit>;

  void resolveAll();

  void resolveAll(std::exception_ptr const&);

  [[nodiscard]] auto empty() const noexcept -> bool;

 private:
  std::vector<futures::Promise<futures::Unit>> _waitForBag;
};

}  // namespace arangodb
