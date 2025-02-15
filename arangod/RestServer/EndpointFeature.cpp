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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#include "EndpointFeature.h"

#include "ApplicationFeatures/ApplicationServer.h"
#include "Basics/application-exit.h"
#include "Logger/LogMacros.h"
#include "Logger/Logger.h"
#include "Logger/LoggerStream.h"
#include "ProgramOptions/ProgramOptions.h"
#include "ProgramOptions/Section.h"
#include "Scheduler/SchedulerFeature.h"

using namespace arangodb::basics;
using namespace arangodb::options;
using namespace arangodb::rest;

namespace arangodb {

EndpointFeature::EndpointFeature(ArangodServer& server)
    : HttpEndpointProvider{server, *this},
      _reuseAddress(true),
      _backlogSize(64) {
  setOptional(true);
  startsAfter<application_features::AqlFeaturePhase, ArangodServer>();

  startsAfter<ServerFeature, ArangodServer>();

  // if our default value is too high, we'll use half of the max value provided
  // by the system
  if (_backlogSize > SOMAXCONN) {
    _backlogSize = SOMAXCONN / 2;
  }
}

void EndpointFeature::collectOptions(std::shared_ptr<ProgramOptions> options) {
  options->addOldOption("server.backlog-size", "tcp.backlog-size");
  options->addOldOption("server.reuse-address", "tcp.reuse-address");

  options->addOption("--server.endpoint",
                     "Endpoint for client requests (e.g. "
                     "`http+tcp://127.0.0.1:8529`, or "
                     "`vst+ssl://192.168.1.1:8529`)",
                     new VectorParameter<StringParameter>(&_endpoints));

  options->addSection("tcp", "TCP features");

  options->addOption(
      "--tcp.reuse-address", "try to reuse TCP port(s)",
      new BooleanParameter(&_reuseAddress),
      arangodb::options::makeDefaultFlags(arangodb::options::Flags::Uncommon));

  options->addOption(
      "--tcp.backlog-size", "listen backlog size",
      new UInt64Parameter(&_backlogSize),
      arangodb::options::makeDefaultFlags(arangodb::options::Flags::Uncommon));
}

void EndpointFeature::validateOptions(std::shared_ptr<ProgramOptions>) {
  if (_backlogSize > SOMAXCONN) {
    LOG_TOPIC("b4d44", WARN, arangodb::Logger::FIXME)
        << "value for --tcp.backlog-size exceeds default system "
           "header SOMAXCONN value "
        << SOMAXCONN << ". trying to use " << SOMAXCONN << " anyway";
  }
}

void EndpointFeature::prepare() {
  buildEndpointLists();

  if (_endpointList.empty()) {
    LOG_TOPIC("2c5f0", FATAL, arangodb::Logger::FIXME)
        << "no endpoints have been specified, giving up, please use the "
           "'--server.endpoint' option";
    FATAL_ERROR_EXIT();
  }
}

std::vector<std::string> EndpointFeature::httpEndpoints() {
  auto httpEntries = _endpointList.all(Endpoint::TransportType::HTTP);
  std::vector<std::string> result;

  for (auto http : httpEntries) {
    auto uri = Endpoint::uriForm(http);

    if (!uri.empty()) {
      result.emplace_back(uri);
    }
  }

  return result;
}

void EndpointFeature::buildEndpointLists() {
  for (auto const& it : _endpoints) {
    bool ok =
        _endpointList.add(it, static_cast<int>(_backlogSize), _reuseAddress);

    if (!ok) {
      LOG_TOPIC("1ddc1", FATAL, arangodb::Logger::FIXME)
          << "invalid endpoint '" << it << "'";
      FATAL_ERROR_EXIT();
    }
  }
}

}  // namespace arangodb
