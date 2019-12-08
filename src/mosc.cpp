#include "maudio/maudio.h"

#include "oscpack/osc/OscPacketListener.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/ip/UdpSocket.h"

#include "mutils/logger/logger.h"

using namespace MUtils;

namespace MAudio
{

namespace
{
std::unique_ptr<UdpListeningReceiveSocket> pReceiver;
}

#define PORT 7000
class ExamplePacketListener : public osc::OscPacketListener
{
protected:
    virtual void ProcessMessage(const osc::ReceivedMessage& m,
        const IpEndpointName& remoteEndpoint)
    {
        (void)remoteEndpoint; // suppress unused parameter warning

        try
        {
            LOG(INFO) << "OSC: " << m.AddressPattern() << ", Args:" << m.ArgumentCount();

            /*
            if (std::strcmp(m.AddressPattern(), "/3/multipushM/2/1") == 0)
            {
                maud.note = "c4";
            }
            else
            {
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                while (arg != m.ArgumentsEnd())
                {
                    if (arg->IsFloat())
                    {
                        LOG(INFO) << "Float: " << arg->AsFloat();
                        maud.fTempScale = arg->AsFloat();
                    }
                    arg++;
                }
            }
            */
            
        }
        catch (osc::Exception& e)
        {
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            LOG(ERROR) << "error while parsing OSC message: " << m.AddressPattern() << ": " << e.what();
        }
    }
};


void mosc_init()
{

    auto listener = new std::thread([]()
    {
        ExamplePacketListener listener;
        pReceiver = std::make_unique<UdpListeningReceiveSocket>(IpEndpointName(IpEndpointName::ANY_ADDRESS, PORT), &listener);
        pReceiver->Run();
    });
}

void mosc_destroy()
{
    pReceiver->Break();
}


} // namespace MAudio
