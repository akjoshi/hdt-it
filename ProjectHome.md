RDF HDT (Header, Dictionary, Triples) is a <b>compact data structure</b> and <b>binary serialization format</b> for RDF that keeps big datasets <b>compressed</b> to save space while maintaining <b>search</b> and <b>browse</b> operations. This makes it an ideal format for storing and sharing RDF datasets on the Web.

This is achieved by organizing and representing the RDF graph in terms of three main components: Header, Dictionary and Triples structure. The Header includes extensible metadata required to describe the RDF data set and its organization. The Dictionary gathers all the terms present in the RDF graph in a manner that permits rapid search and high levels of compression. The Triples component represents the structure of relationships of the RDF graph in a compressed form.

Some facts about HDT:
<ul>
<blockquote><li><b>The size of the files is smaller</b> than other RDF serialization formats. This means less bandwidth costs for the provider, but also less waiting time for the consumers to download.</li>
<li><b>The HDT file is already indexed</b>. The users of RDF dumps want to do something useful with the data. By using HDT, they download the file and start browsing/querying in minutes, instead of wasting time using parsing and indexing tools that they never remember how to setup and tune. </li>
<li><b>High performance querying</b>. Usually the bottleneck of databases is slow disk access. The internal compression techniques of HDT allow that most part of the data (or even the whole dataset) can be kept in main memory, which is several orders of magnitude faster than disks.</li>
<li><b>Highly concurrent</b>. HDT is read-only, so it can dispatch many queries per second using multiple threads.</li>
<li><b>The format is open</b> and in progress of standardization (<a href='http://www.w3.org/Submission/2011/03/'>W3C HDT Member Submission</a>). This ensures that anyone on the Web can generate and consume files, or even write their own implementation.</li>
<li><b>The libraries are open source (LGPL)</b>. You can adapt the libraries to your needs, and the community can spot and fix issues.</li>
</ul></blockquote>

Some use cases where HDT stands out:
<ul>
<blockquote><li><b>Sharing RDF data on the Web</b>. Your dataset is compact and ready to be used by the consumers.</li>
<li><b>Data Analysis</b> and <b>Visualization</b>. Tasks such as statistics extraction, data mining and clustering, require exhaustive traversals over the data. In these cases is much more convenient to have all the data locally rather than rely on slow remote SPARQL endpoints.</li>
<li><b>Setting up a mirror of an endpoint</b>. Just download the HDT and launch the HDT Sparql Endpoint software.</li>
<li><b>Smartphones and Embedded Devices</b>. They are very restricted in terms of CPU and Storage so it is important to use appropriate data formats to use resources efficiently when developing Semantic Applications.</li>
<li><b>Federated Querying</b>. Federated Query Engines can directly download big parts of frequently-accessed data sources and keep them locally cached. Thanks to HDT both the transmission and storage are cheap, and querying the cached block is very fast.</li>
</ul>