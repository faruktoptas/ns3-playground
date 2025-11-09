#include "ns3/core-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("HelloWorldLogging");

int main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  LogComponentEnable ("HelloWorldLogging", LOG_LEVEL_INFO);

  NS_LOG_INFO ("Hello World - NS-3 Logging Simulation");
  NS_LOG_INFO ("==============================================");
  NS_LOG_INFO ("This is a simple example demonstrating ns-3 logging capabilities.");
  
  NS_LOG_INFO ("Creating simulation environment...");
  
  // Simple simulation time setup
  double simTime = 5.0;
  
  NS_LOG_INFO ("Simulation will run for " << simTime << " seconds");
  
  // Schedule some events to demonstrate logging
  Simulator::Schedule (Seconds (1.0), [] () {
    NS_LOG_INFO ("Event at t=1.0s: First scheduled event executed");
  });
  
  Simulator::Schedule (Seconds (2.5), [] () {
    NS_LOG_INFO ("Event at t=2.5s: Second scheduled event executed");
  });
  
  Simulator::Schedule (Seconds (4.0), [] () {
    NS_LOG_INFO ("Event at t=4.0s: Third scheduled event executed");
  });
  
  NS_LOG_INFO ("Starting simulation...");
  
  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();
  
  NS_LOG_INFO ("Simulation completed successfully!");
  NS_LOG_INFO ("Total simulation time: " << simTime << " seconds");
  
  Simulator::Destroy ();
  
  return 0;
}