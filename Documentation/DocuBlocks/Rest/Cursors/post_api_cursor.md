
@startDocuBlock post_api_cursor
@brief create a cursor and return the first results

@RESTHEADER{POST /_api/cursor, Create cursor, createQueryCursor}

@RESTHEADERPARAMETERS

@RESTHEADERPARAM{x-arango-allow-dirty-read,boolean,optional}
Set this header to `true` to allow the Coordinator to ask any shard replica for
the data, not only the shard leader. This may result in "dirty reads".

The header is ignored if this operation is part of a Stream Transaction
(`x-arango-trx-id` header). The header set when creating the transaction decides
about dirty reads for the entire transaction, not the individual read operations.

@RESTHEADERPARAM{x-arango-trx-id,string,optional}
To make this operation a part of a Stream Transaction, set this header to the
transaction ID returned by the `POST /_api/transaction/begin` call.

@RESTBODYPARAM{query,string,required,string}
contains the query string to be executed

@RESTBODYPARAM{count,boolean,optional,}
indicates whether the number of documents in the result set should be returned in
the "count" attribute of the result.
Calculating the "count" attribute might have a performance impact for some queries
in the future so this option is turned off by default, and "count"
is only returned when requested.

@RESTBODYPARAM{batchSize,integer,optional,int64}
maximum number of result documents to be transferred from
the server to the client in one roundtrip. If this attribute is
not set, a server-controlled default value will be used. A *batchSize* value of
*0* is disallowed.

@RESTBODYPARAM{ttl,integer,optional,int64}
The time-to-live for the cursor (in seconds). If the result set is small enough
(less than or equal to `batchSize`) then results are returned right away.
Otherwise they are stored in memory and will be accessible via the cursor with
respect to the `ttl`. The cursor will be removed on the server automatically
after the specified amount of time. This is useful to ensure garbage collection
of cursors that are not fully fetched by clients. If not set, a server-defined
value will be used (default: 30 seconds).

@RESTBODYPARAM{cache,boolean,optional,}
flag to determine whether the AQL query results cache
shall be used. If set to *false*, then any query cache lookup will be skipped
for the query. If set to *true*, it will lead to the query cache being checked
for the query if the query cache mode is either *on* or *demand*.

@RESTBODYPARAM{memoryLimit,integer,optional,int64}
the maximum number of memory (measured in bytes) that the query is allowed to
use. If set, then the query will fail with error "resource limit exceeded" in
case it allocates too much memory. A value of *0* indicates that there is no
memory limit.

@RESTBODYPARAM{bindVars,array,optional,object}
key/value pairs representing the bind parameters.

@RESTBODYPARAM{options,object,optional,post_api_cursor_opts}
key/value object with extra options for the query.

@RESTSTRUCT{fullCount,post_api_cursor_opts,boolean,optional,}
if set to *true* and the query contains a *LIMIT* clause, then the
result will have an *extra* attribute with the sub-attributes *stats*
and *fullCount*, `{ ... , "extra": { "stats": { "fullCount": 123 } } }`.
The *fullCount* attribute will contain the number of documents in the result before the
last top-level LIMIT in the query was applied. It can be used to count the number of
documents that match certain filter criteria, but only return a subset of them, in one go.
It is thus similar to MySQL's *SQL_CALC_FOUND_ROWS* hint. Note that setting the option
will disable a few LIMIT optimizations and may lead to more documents being processed,
and thus make queries run longer. Note that the *fullCount* attribute may only
be present in the result if the query has a top-level LIMIT clause and the LIMIT
clause is actually used in the query.

@RESTSTRUCT{fillBlockCache,post_api_cursor_opts,boolean,optional,}
if set to *true* or not specified, this will make the query store the data it 
reads via the RocksDB storage engine in the RocksDB block cache. This is usually 
the desired behavior. The option can be set to *false* for queries that are
known to either read a lot of data which would thrash the block cache, or for queries
that read data which are known to be outside of the hot set. By setting the option
to *false*, data read by the query will not make it into the RocksDB block cache if
not already in there, thus leaving more room for the actual hot set.

@RESTSTRUCT{maxPlans,post_api_cursor_opts,integer,optional,int64}
Limits the maximum number of plans that are created by the AQL query optimizer.

@RESTSTRUCT{maxNodesPerCallstack,post_api_cursor_opts,integer,optional,int64}
The number of execution nodes in the query plan after that stack splitting is
performed to avoid a potential stack overflow. Defaults to the configured value
of the startup option `--query.max-nodes-per-callstack`.

This option is only useful for testing and debugging and normally does not need
any adjustment.

@RESTSTRUCT{maxWarningCount,post_api_cursor_opts,integer,optional,int64}
Limits the maximum number of warnings a query will return. The number of warnings
a query will return is limited to 10 by default, but that number can be increased
or decreased by setting this attribute.

@RESTSTRUCT{failOnWarning,post_api_cursor_opts,boolean,optional,}
When set to *true*, the query will throw an exception and abort instead of producing
a warning. This option should be used during development to catch potential issues
early. When the attribute is set to *false*, warnings will not be propagated to
exceptions and will be returned with the query result.
There is also a server configuration option `--query.fail-on-warning` for setting the
default value for *failOnWarning* so it does not need to be set on a per-query level.

@RESTSTRUCT{stream,post_api_cursor_opts,boolean,optional,}
Can be enabled to execute the query lazily. If set to *true*, then the query is
executed as long as necessary to produce up to `batchSize` results. These
results are returned immediately and the query is suspended until the client
asks for the next batch (if there are more results). Depending on the query
this can mean that the first results will be available much faster and that
less memory is needed because the server only needs to store a subset of
results at a time. Read-only queries can benefit the most, unless `SORT`
without index or `COLLECT` are involved that make it necessary to process all
documents before a partial result can be returned. It is advisable to only use
this option for queries without exclusive locks.

Remarks:
- The query will hold resources until it ends (such as RocksDB snapshots, which
  prevents compaction to some degree). Writes will be in memory until the query
  is committed.
- If existing documents are modified, then write locks are held on these
  documents and other queries trying to modify the same documents will fail
  because of this conflict.
- A streaming query may fail late because of a conflict or for other reasons
  after some batches were already returned successfully, possibly rendering the
  results up to that point meaningless.
- The query options `cache`, `count` and `fullCount` are not supported for
  streaming queries.
- Query statistics, profiling data and warnings are delivered as part of the
  last batch.

If the `stream` option is *false* (default), then the complete result of the
query is calculated before any of it is returned to the client. The server
stores the full result in memory (on the contacted Coordinator if in a cluster).
All other resources are freed immediately (locks, RocksDB snapshots). The query
will fail before it returns results in case of a conflict.

@RESTSTRUCT{optimizer,post_api_cursor_opts,object,optional,post_api_cursor_opts_optimizer}
Options related to the query optimizer.

@RESTSTRUCT{rules,post_api_cursor_opts_optimizer,array,optional,string}
A list of to-be-included or to-be-excluded optimizer rules can be put into this
attribute, telling the optimizer to include or exclude specific rules. To disable
a rule, prefix its name with a `-`, to enable a rule, prefix it with a `+`. There is
also a pseudo-rule `all`, which matches all optimizer rules. `-all` disables all rules.

@RESTSTRUCT{profile,post_api_cursor_opts,integer,optional,}
If set to `true` or `1`, then the additional query profiling information is returned
in the `profile` sub-attribute of the `extra` return attribute, unless the query result
is served from the query cache. If set to `2`, the query includes execution stats
per query plan node in `stats.nodes` sub-attribute of the `extra` return attribute.
Additionally, the query plan is returned in the `extra.plan` sub-attribute.

@RESTSTRUCT{satelliteSyncWait,post_api_cursor_opts,number,optional,double}
This *Enterprise Edition* parameter allows to configure how long a DB-Server has time
to bring the SatelliteCollections involved in the query into sync.
The default value is `60.0` seconds. When the maximal time is reached, the query
is stopped.

@RESTSTRUCT{maxRuntime,post_api_cursor_opts,number,optional,double}
The query has to be executed within the given runtime or it is killed.
The value is specified in seconds. The default value is `0.0` (no timeout).

@RESTSTRUCT{maxTransactionSize,post_api_cursor_opts,integer,optional,int64}
The transaction size limit in bytes.

@RESTSTRUCT{intermediateCommitSize,post_api_cursor_opts,integer,optional,int64}
The maximum total size of operations after which an intermediate commit is performed
automatically.

@RESTSTRUCT{intermediateCommitCount,post_api_cursor_opts,integer,optional,int64}
The maximum number of operations after which an intermediate commit is performed
automatically.

@RESTSTRUCT{skipInaccessibleCollections,post_api_cursor_opts,boolean,optional,}
Let AQL queries (especially graph traversals) treat collection to which a user
has no access rights for as if these collections are empty. Instead of returning a
forbidden access error, your queries execute normally. This is intended to help
with certain use-cases: A graph contains several collections and different users
execute AQL queries on that graph. You can naturally limit the accessible
results by changing the access rights of users on collections.

This feature is only available in the Enterprise Edition.

@RESTDESCRIPTION
The query details include the query string plus optional query options and
bind parameters. These values need to be passed in a JSON representation in
the body of the POST request.

@RESTRETURNCODES

@RESTRETURNCODE{201}
is returned if the result set can be created by the server.

@RESTREPLYBODY{error,boolean,required,}
A flag to indicate that an error occurred (`false` in this case).

@RESTREPLYBODY{code,integer,required,integer}
The HTTP status code.

@RESTREPLYBODY{result,array,optional,}
An array of result documents (might be empty if query has no results).

@RESTREPLYBODY{hasMore,boolean,required,}
A boolean indicator whether there are more results
available for the cursor on the server.

@RESTREPLYBODY{count,integer,optional,int64}
The total number of result documents available (only
available if the query was executed with the `count` attribute set).

@RESTREPLYBODY{id,string,optional,string}
The ID of a temporary cursor created on the server for fetching more result batches.

@RESTREPLYBODY{extra,object,optional,post_api_cursor_extra}
An optional JSON object with extra information about the query result.

@RESTSTRUCT{warnings,post_api_cursor_extra,array,required,post_api_cursor_extra_warnings}
A list of query warnings.

@RESTSTRUCT{code,post_api_cursor_extra_warnings,integer,required,}
An error code.

@RESTSTRUCT{message,post_api_cursor_extra_warnings,string,required,}
A description of the problem.

@RESTSTRUCT{stats,post_api_cursor_extra,object,required,post_api_cursor_extra_stats}
An object with query statistics.

@RESTSTRUCT{writesExecuted,post_api_cursor_extra_stats,integer,required,}
The total number of data-modification operations successfully executed.

@RESTSTRUCT{writesIgnored,post_api_cursor_extra_stats,integer,required,}
The total number of data-modification operations that were unsuccessful,
but have been ignored because of the `ignoreErrors` query option.

@RESTSTRUCT{scannedFull,post_api_cursor_extra_stats,integer,required,}
The total number of documents iterated over when scanning a collection 
without an index. Documents scanned by subqueries are included in the result, but
operations triggered by built-in or user-defined AQL functions are not.

@RESTSTRUCT{scannedIndex,post_api_cursor_extra_stats,integer,required,}
The total number of documents iterated over when scanning a collection using
an index. Documents scanned by subqueries are included in the result, but operations
triggered by built-in or user-defined AQL functions are not.

@RESTSTRUCT{cursorsCreated,post_api_cursor_extra_stats,integer,required,}
The total number of cursor objects created during query execution. Cursor
objects are created for index lookups.

@RESTSTRUCT{cursorsRearmed,post_api_cursor_extra_stats,integer,required,}
The total number of times an existing cursor object was repurposed.
Repurposing an existing cursor object is normally more efficient compared to destroying an
existing cursor object and creating a new one from scratch.

@RESTSTRUCT{cacheHits,post_api_cursor_extra_stats,integer,required,}
The total number of index entries read from in-memory caches for indexes
of type edge or persistent. This value is only non-zero when reading from indexes
that have an in-memory cache enabled, and when the query allows using the in-memory
cache (i.e. using equality lookups on all index attributes).

@RESTSTRUCT{cacheMisses,post_api_cursor_extra_stats,integer,required,}
The total number of cache read attempts for index entries that could not
be served from in-memory caches for indexes of type edge or persistent. This value
is only non-zero when reading from indexes that have an in-memory cache enabled, the
query allows using the in-memory cache (i.e. using equality lookups on all index attributes)
and the looked up values are not present in the cache.

@RESTSTRUCT{filtered,post_api_cursor_extra_stats,integer,required,}
The total number of documents removed after executing a filter condition
in a `FilterNode` or another node that post-filters data. Note that nodes of the
`IndexNode` type can also filter documents by selecting only the required index range 
from a collection, and the `filtered` value only indicates how much filtering was done by a
post filter in the `IndexNode` itself or following `FilterNode` nodes.
Nodes of the `EnumerateCollectionNode` and `TraversalNode` types can also apply
filter conditions and can report the number of filtered documents.

@RESTSTRUCT{httpRequests,post_api_cursor_extra_stats,integer,required,}
The total number of cluster-internal HTTP requests performed.

@RESTSTRUCT{fullCount,post_api_cursor_extra_stats,integer,optional,}
The total number of documents that matched the search condition if the query's
final top-level `LIMIT` operation were not present.
This attribute may only be returned if the `fullCount` option was set when starting the 
query and only contains a sensible value if the query contains a `LIMIT` operation on
the top level.

@RESTSTRUCT{executionTime,post_api_cursor_extra_stats,number,required,}
The query execution time (wall-clock time) in seconds.

@RESTSTRUCT{peakMemoryUsage,post_api_cursor_extra_stats,integer,required,}
The maximum memory usage of the query while it was running. In a cluster,
the memory accounting is done per shard, and the memory usage reported is the peak
memory usage value from the individual shards.
Note that to keep things lightweight, the per-query memory usage is tracked on a relatively 
high level, not including any memory allocator overhead nor any memory used for temporary
results calculations (e.g. memory allocated/deallocated inside AQL expressions and function 
calls).

@RESTSTRUCT{intermediateCommits,post_api_cursor_extra_stats,integer,optional,}
The number of intermediate commits performed by the query. This is only non-zero
for write queries, and only for queries that reached either the `intermediateCommitSize`
or `intermediateCommitCount` thresholds. Note: in a cluster, intermediate commits can happen
on each participating DB-Server.

@RESTSTRUCT{nodes,post_api_cursor_extra_stats,array,optional,post_api_cursor_extra_stats_nodes}
When the query is executed with the `profile` option set to at least `2`,
then this attribute contains runtime statistics per query execution node.
For a human readable output, you can execute
`db._profileQuery(<query>, <bind-vars>)` in arangosh.

@RESTSTRUCT{id,post_api_cursor_extra_stats_nodes,integer,required,}
The execution node ID to correlate the statistics with the `plan` returned in
the `extra` attribute.

@RESTSTRUCT{calls,post_api_cursor_extra_stats_nodes,integer,required,}
The number of calls to this node.

@RESTSTRUCT{items,post_api_cursor_extra_stats_nodes,integer,required,}
The number of items returned by this node. Items are the temporary results
returned at this stage.

@RESTSTRUCT{runtime,post_api_cursor_extra_stats_nodes,number,required,}
The execution time of this node in seconds.

@RESTSTRUCT{profile,post_api_cursor_extra,object,optional,post_api_cursor_extra_profile}
The duration of the different query execution phases in seconds.

@RESTSTRUCT{initializing,post_api_cursor_extra_profile,number,required,}
@RESTSTRUCT{parsing,post_api_cursor_extra_profile,number,required,}
@RESTSTRUCT{optimizing ast,post_api_cursor_extra_profile,number,required,}
@RESTSTRUCT{loading collections,post_api_cursor_extra_profile,number,required,}
@RESTSTRUCT{instantiating plan,post_api_cursor_extra_profile,number,required,}
@RESTSTRUCT{optimizing plan,post_api_cursor_extra_profile,number,required,}
@RESTSTRUCT{executing,post_api_cursor_extra_profile,number,required,}
@RESTSTRUCT{finalizing,post_api_cursor_extra_profile,number,required,}

@RESTSTRUCT{plan,post_api_cursor_extra,object,optional,post_api_cursor_extra_plan}
The execution plan.

@RESTSTRUCT{nodes,post_api_cursor_extra_plan,array,required,object}
A nested list of the execution plan nodes.

@RESTSTRUCT{rules,post_api_cursor_extra_plan,array,required,string}
A list with the names of the applied optimizer rules.

@RESTSTRUCT{collections,post_api_cursor_extra_plan,array,required,post_api_cursor_extra_plan_collections}
A list of the collections involved in the query. The list only includes the
collections that can statically be determined at query compile time.

@RESTSTRUCT{name,post_api_cursor_extra_plan_collections,string,required,}
The collection name.

@RESTSTRUCT{type,post_api_cursor_extra_plan_collections,string,required,}
How the collection is used. Can be `"read"`, `"write"`, or `"exclusive"`.

@RESTSTRUCT{variables,post_api_cursor_extra_plan,array,required,object}
All of the query variables, including user-created and internal ones.

@RESTSTRUCT{estimatedCost,post_api_cursor_extra_plan,integer,required,}
The estimated cost of the query.

@RESTSTRUCT{estimatedNrItems,post_api_cursor_extra_plan,integer,required,}
The estimated number of results.

@RESTSTRUCT{isModificationQuery,post_api_cursor_extra_plan,boolean,required,}
Whether the query contains write operations.

@RESTREPLYBODY{cached,boolean,required,}
A boolean flag indicating whether the query result was served
from the query cache or not. If the query result is served from the query
cache, the `extra` return attribute will not contain any `stats` sub-attribute
and no `profile` sub-attribute.

@RESTRETURNCODE{400}
is returned if the JSON representation is malformed or the query specification is
missing from the request.

If the JSON representation is malformed or the query specification is
missing from the request, the server will respond with *HTTP 400*.

The body of the response will contain a JSON object with additional error
details. The object has the following attributes:

@RESTREPLYBODY{error,boolean,required,}
boolean flag to indicate that an error occurred (*true* in this case)

@RESTREPLYBODY{code,integer,required,int64}
the HTTP status code

@RESTREPLYBODY{errorNum,integer,required,int64}
the server error number

@RESTREPLYBODY{errorMessage,string,required,string}
A descriptive error message.

If the query specification is complete, the server will process the query. If an
error occurs during query processing, the server will respond with *HTTP 400*.
Again, the body of the response will contain details about the error.

@RESTRETURNCODE{404}
The server will respond with *HTTP 404* in case a non-existing collection is
accessed in the query.

@RESTRETURNCODE{405}
The server will respond with *HTTP 405* if an unsupported HTTP method is used.

@RESTRETURNCODE{410}
The server will respond with *HTTP 410* if a server which processes the query
or is the leader for a shard which is used in the query stops responding, but 
the connection has not been closed.

@RESTRETURNCODE{503}
The server will respond with *HTTP 503* if a server which processes the query
or is the leader for a shard which is used in the query is down, either for 
going through a restart, a failure or connectivity issues.

@EXAMPLES

Execute a query and extract the result in a single go

@EXAMPLE_ARANGOSH_RUN{RestCursorCreateCursorForLimitReturnSingle}
    var cn = "products";
    db._drop(cn);
    db._create(cn);

    db.products.save({"hello1":"world1"});
    db.products.save({"hello2":"world1"});

    var url = "/_api/cursor";
    var body = {
      query: "FOR p IN products LIMIT 2 RETURN p",
      count: true,
      batchSize: 2
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 201);

    logJsonResponse(response);
  ~ db._drop(cn);
@END_EXAMPLE_ARANGOSH_RUN

Execute a query and extract a part of the result

@EXAMPLE_ARANGOSH_RUN{RestCursorCreateCursorForLimitReturn}
    var cn = "products";
    db._drop(cn);
    db._create(cn);

    db.products.save({"hello1":"world1"});
    db.products.save({"hello2":"world1"});
    db.products.save({"hello3":"world1"});
    db.products.save({"hello4":"world1"});
    db.products.save({"hello5":"world1"});

    var url = "/_api/cursor";
    var body = {
      query: "FOR p IN products LIMIT 5 RETURN p",
      count: true,
      batchSize: 2
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 201);

    logJsonResponse(response);
  ~ db._drop(cn);
@END_EXAMPLE_ARANGOSH_RUN

Using the query option "fullCount"

@EXAMPLE_ARANGOSH_RUN{RestCursorCreateCursorOption}
    var url = "/_api/cursor";
    var body = {
      query: "FOR i IN 1..1000 FILTER i > 500 LIMIT 10 RETURN i",
      count: true,
      options: {
        fullCount: true
      }
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 201);

    logJsonResponse(response);
@END_EXAMPLE_ARANGOSH_RUN

Enabling and disabling optimizer rules

@EXAMPLE_ARANGOSH_RUN{RestCursorOptimizerRules}
    var url = "/_api/cursor";
    var body = {
      query: "FOR i IN 1..10 LET a = 1 LET b = 2 FILTER a + b == 3 RETURN i",
      count: true,
      options: {
        maxPlans: 1,
        optimizer: {
          rules: [ "-all", "+remove-unnecessary-filters" ]
        }
      }
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 201);

    logJsonResponse(response);
@END_EXAMPLE_ARANGOSH_RUN

Execute instrumented query and return result together with
execution plan and profiling information

@EXAMPLE_ARANGOSH_RUN{RestCursorProfileQuery}
    var url = "/_api/cursor";
    var body = {
      query: "LET s = SLEEP(0.25) LET t = SLEEP(0.5) RETURN 1",
      count: true,
      options: {
        profile: 2
      }
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 201);

    logJsonResponse(response);
@END_EXAMPLE_ARANGOSH_RUN

Execute a data-modification query and retrieve the number of
modified documents

@EXAMPLE_ARANGOSH_RUN{RestCursorDeleteQuery}
    var cn = "products";
    db._drop(cn);
    db._create(cn);

    db.products.save({"hello1":"world1"});
    db.products.save({"hello2":"world1"});

    var url = "/_api/cursor";
    var body = {
      query: "FOR p IN products REMOVE p IN products"
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 201);
    assert(JSON.parse(response.body).extra.stats.writesExecuted === 2);
    assert(JSON.parse(response.body).extra.stats.writesIgnored === 0);

    logJsonResponse(response);
  ~ db._drop(cn);
@END_EXAMPLE_ARANGOSH_RUN

Execute a data-modification query with option *ignoreErrors*

@EXAMPLE_ARANGOSH_RUN{RestCursorDeleteIgnore}
    var cn = "products";
    db._drop(cn);
    db._create(cn);

    db.products.save({ _key: "foo" });

    var url = "/_api/cursor";
    var body = {
      query: "REMOVE 'bar' IN products OPTIONS { ignoreErrors: true }"
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 201);
    assert(JSON.parse(response.body).extra.stats.writesExecuted === 0);
    assert(JSON.parse(response.body).extra.stats.writesIgnored === 1);

    logJsonResponse(response);
  ~ db._drop(cn);
@END_EXAMPLE_ARANGOSH_RUN

Bad query - Missing body

@EXAMPLE_ARANGOSH_RUN{RestCursorCreateCursorMissingBody}
    var url = "/_api/cursor";

    var response = logCurlRequest('POST', url, '');

    assert(response.code === 400);

    logJsonResponse(response);
@END_EXAMPLE_ARANGOSH_RUN

Bad query - Unknown collection

@EXAMPLE_ARANGOSH_RUN{RestCursorCreateCursorUnknownCollection}
    var url = "/_api/cursor";
    var body = {
      query: "FOR u IN unknowncoll LIMIT 2 RETURN u",
      count: true,
      batchSize: 2
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 404);

    logJsonResponse(response);
@END_EXAMPLE_ARANGOSH_RUN

Bad query - Execute a data-modification query that attempts to remove a non-existing
document

@EXAMPLE_ARANGOSH_RUN{RestCursorDeleteQueryFail}
    var cn = "products";
    db._drop(cn);
    db._create(cn);

    db.products.save({ _key: "bar" });

    var url = "/_api/cursor";
    var body = {
      query: "REMOVE 'foo' IN products"
    };

    var response = logCurlRequest('POST', url, body);

    assert(response.code === 404);

    logJsonResponse(response);
  ~ db._drop(cn);
@END_EXAMPLE_ARANGOSH_RUN

@endDocuBlock
