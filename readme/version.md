1.
void 
InnetworkAggregationInterface::TriggerHandleRead () {
    for (auto item : socketPool) {
        // Zhuoxu: only receive data from the server
        if ( socketPool[item.first]->GetObject<TCPserver>() ){
            Ptr<TCPserver> server = socketPool[item.first]->GetObject<TCPserver>();
            server->CallHandleRead();
        }
    }
    return;
}

This function should be added when the ReceiveDataFrom() function finish the iterChunk erase operation.