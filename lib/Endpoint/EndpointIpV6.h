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

#include "Endpoint/EndpointIp.h"

#include "Basics/StringUtils.h"

namespace arangodb {
class EndpointIpV6 final : public EndpointIp {
 public:
  EndpointIpV6(EndpointType, TransportType, EncryptionType, int, bool,
               std::string const&, uint16_t);

 public:
  int domain() const override { return AF_INET6; }
  bool isBroadcastBind() const override;

  std::string hostAndPort() const override {
    return '[' + host() + "]:" + arangodb::basics::StringUtils::itoa(port());
  }
};
}  // namespace arangodb
