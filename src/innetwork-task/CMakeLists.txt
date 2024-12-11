build_lib(
  LIBNAME innetwork-task
  SOURCE_FILES
    helper/parameter.cc
    helper/vectorop.cc
    model/aggregator.cc
    model/consumer.cc
    model/InnetworkAggregationInterface.cc
    model/producer.cc
    
  HEADER_FILES
    helper/parameter.h
    helper/vectorop.h
    model/aggregator.h
    model/consumer.h
    model/InnetworkAggregationInterface.h
    model/producer.h
    
  LIBRARIES_TO_LINK ${libinternet}
                    ${libapplications}
                    ${libflow-monitor}
                    ${libpoint-to-point}
)
