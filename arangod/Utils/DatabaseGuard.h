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
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Basics/Common.h"
#include "VocBase/vocbase.h"

namespace arangodb {
class DatabaseFeature;

struct IDatabaseGuard {
  virtual ~IDatabaseGuard() = default;
  [[nodiscard]] virtual TRI_vocbase_t& database() const noexcept = 0;
};

/// @brief Scope guard for a database, ensures that it is not
///        dropped while still using it.
class DatabaseGuard final : public IDatabaseGuard {
 public:
  DatabaseGuard(DatabaseGuard&&) = delete;
  DatabaseGuard(DatabaseGuard const&) = delete;
  DatabaseGuard& operator=(DatabaseGuard const&) = delete;

  /// @brief create guard on existing db
  explicit DatabaseGuard(TRI_vocbase_t& vocbase);

  /// @brief create the guard, using a database id
  explicit DatabaseGuard(DatabaseFeature& feature, TRI_voc_tick_t id);

  /// @brief create the guard, using a database name
  explicit DatabaseGuard(DatabaseFeature& feature, std::string const& name);

  /// @brief destroy the guard
  ~DatabaseGuard() {
    TRI_ASSERT(!_vocbase.isDangling());
    _vocbase.release();
  }

  /// @brief return the database pointer
  TRI_vocbase_t& database() const noexcept final { return _vocbase; }
  auto operator->() const noexcept -> TRI_vocbase_t const* { return &_vocbase; }
  auto operator->() noexcept -> TRI_vocbase_t* { return &_vocbase; }

 private:
  /// @brief pointer to database
  TRI_vocbase_t& _vocbase;
};

}  // namespace arangodb
