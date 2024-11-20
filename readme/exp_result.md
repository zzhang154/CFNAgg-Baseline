Experiment Result for v4.0:

producer-10, iteration-150
ns3::Simulator::Schedule(ns3::MilliSeconds(10), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.200000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 200ms

ns3::Simulator::Schedule(ns3::MilliSeconds(3), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.202000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 202ms

producer-50, iteration-150
ns3::Simulator::Schedule(ns3::MilliSeconds(5), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.215000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 215ms

ns3::Simulator::Schedule(ns3::MilliSeconds(10), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.210000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 210ms

ns3::Simulator::Schedule(ns3::MilliSeconds(3), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.193000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 193ms


----------result for vsize = 300,000 ------------
 Next, we extend the data set to vsize=300,000

producer-10
+2.280000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 280ms

producer-50
+2.313000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 313ms

