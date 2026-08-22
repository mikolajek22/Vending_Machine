#include <core/transportWorker.hpp>

#include <algorithm>

namespace vending
{



TransportWorker::TransportWorker(ITransactionJournal &transactionJournal ,ITransport &transport) : 
                _transactionJournal(transactionJournal), _transport(transport)
{

}
TransportWorker::~TransportWorker()
{
    stop();
}
/* thread - start / stop */
void TransportWorker::start(std::chrono::milliseconds interval)
{
    _threadEnabled = true;
    _thread = std::thread([this, interval] { _thread_entry(interval); });
}
void TransportWorker::stop()
{
    _threadEnabled = false;
    if (_thread.joinable())
        _thread.join();
}

bool TransportWorker::sync()
{
    auto pending = _transactionJournal.findNotSynced();
    bool ret = true;
    for (const auto &transaction : pending)
    {
        if(_transport.send(transaction))
            _transactionJournal.syncDone(transaction.uuid);
        else
        {
            ret = false;
            break;
        }
            
    }
    return ret;
}
void TransportWorker::_thread_entry(std::chrono::milliseconds interval)
{
    const auto maxBackoff = std::chrono::milliseconds(30000);
    auto backoff = interval;
    while(_threadEnabled)
    {
        if (sync())
            backoff = interval;
        else
            backoff = std::min(backoff * 2, maxBackoff);

        std::this_thread::sleep_for(backoff);
    }
}


} /* namespace vending */