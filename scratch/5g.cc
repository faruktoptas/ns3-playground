#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("5GSimulation");

int main(int argc, char *argv[])
{
  Time::SetResolution(Time::NS);
  LogComponentEnable("5GSimulation", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
  
  // === 5G PARAMETRELERİ ===
  uint32_t numUes = 3;              // 3 kullanıcı cihazı
  uint32_t numGnbs = 1;             // 1 baz istasyonu
  double simTime = 15.0;            // 15 saniye simülasyon
  double mmWaveFreq = 28.0;         // 28 GHz (mmWave band)
  double bandwidth = 100.0;         // 100 MHz bant genişliği (5G özellikleri)
  
  NS_LOG_INFO("╔════════════════════════════════════════════╗");
  NS_LOG_INFO("║   5G (LTE Tabanlı) Ağ Simülasyonu         ║");
  NS_LOG_INFO("║════════════════════════════════════════════║");
  NS_LOG_INFO("║ Frekans: " << mmWaveFreq << " GHz (mmWave Band)");
  NS_LOG_INFO("║ Bant Genişliği: " << bandwidth << " MHz");
  NS_LOG_INFO("║ Simülasyon Süresi: " << simTime << " saniye");
  NS_LOG_INFO("║ gNB (Baz Istasyonu): " << numGnbs);
  NS_LOG_INFO("║ UE (Cihaz): " << numUes);
  NS_LOG_INFO("╚════════════════════════════════════════════╝");
  
  // Baz istasyonları oluştur
  NodeContainer gnbNodes;
  gnbNodes.Create(numGnbs);
  
  // Cihazları oluştur
  NodeContainer ueNodes;
  ueNodes.Create(numUes);
  
  NS_LOG_INFO("[NODE] gNB ve UE node'ları oluşturuldu");
  
  // Mobilite model olarak sabit konum seçilir
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  
  // Baz istasyonu ve cihazların konumlarını ver.  Aşağıdaki konumlarda sabit kalacaktir. 
  Ptr<ListPositionAllocator> posAlloc = CreateObject<ListPositionAllocator>();
  posAlloc->Add(Vector(0.0, 0.0, 30.0));           // gNB (yüksekte 30m)
  posAlloc->Add(Vector(100.0, 0.0, 1.5));          // UE1 (100m uzakta)
  posAlloc->Add(Vector(150.0, 80.0, 1.5));         // UE2 (150m uzakta, farklı konum)
  posAlloc->Add(Vector(200.0, -60.0, 1.5));        // UE3 (200m uzakta)
  
  // Mobilite modelini node'lara uygula
  mobility.SetPositionAllocator(posAlloc);
  NodeContainer allNodes(gnbNodes, ueNodes);
  mobility.Install(allNodes);

  
  // LTE helper oluştur
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
  
  // 5G özellikleri: Kanal modeli
  // LogDistance = uzaklaştıkça sinyal kaybı artar (gerçekçi)
  lteHelper->SetAttribute("PathlossModel", 
                         StringValue("ns3::LogDistancePropagationLossModel"));
  

  
  // Baz istasyonunu ağa yükle
  NetDeviceContainer gnbNetDevs = lteHelper->InstallEnbDevice(gnbNodes);
  NS_LOG_INFO("[AĞDAN] gNB ağ cihazları kuruldu");
  
  // Cihazları ağa yükle
  NetDeviceContainer ueNetDevs = lteHelper->InstallUeDevice(ueNodes);
  NS_LOG_INFO("[AĞDAN] UE ağ cihazları kuruldu");
  
  // Cihazlara ağa bağlanma özelliği ekle
  InternetStackHelper internet;
  internet.Install(ueNodes);
  NS_LOG_INFO("[STACK] TCP/IP stack yüklendi");
  
  
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer ueIpIface = ipv4.Assign(ueNetDevs);
  
  NS_LOG_INFO("[IP] 5G Ağı Adresleri:");
  for(uint32_t i = 0; i < numUes; i++) {
    NS_LOG_INFO("     UE" << i << ": " << ueIpIface.GetAddress(i));
  }
  
  // === UE-gNB BAĞLANTISI ===
  lteHelper->Attach(ueNetDevs, gnbNetDevs.Get(0));
  NS_LOG_INFO("[BAĞLANTI] UE'ler gNB'ye bağlandı");
  
  // === DATA BEARER (Veri Taşıyıcısı) ===
  // Video streaming için QoS ayarı (5G'ye benzer yüksek hız)
  lteHelper->ActivateDataRadioBearer(ueNetDevs, 
                                     EpsBearer(EpsBearer::NGBR_VIDEO_TCP_DEFAULT));
  NS_LOG_INFO("[BEARER] Data Radio Bearer aktifleştirildi");
  
  // === UYGULAMA 1: UDP Echo Sunucusu ===
  // UE0'da sunucu çalışır
  uint16_t port = 9;
  UdpEchoServerHelper echoServer(port);
  ApplicationContainer serverApps = echoServer.Install(ueNodes.Get(0));
  serverApps.Start(Seconds(0.5));
  serverApps.Stop(Seconds(simTime));
  NS_LOG_INFO("[SUNUCU] UDP Echo Sunucusu Port 9'da açılmış (UE0)");
  
  // === UYGULAMA 2 & 3: UDP Echo İstemcileri ===
  // UE1 ve UE2 UE0'a veri gönderir
  for(uint32_t i = 1; i < numUes; i++) {
    UdpEchoClientHelper echoClient(ueIpIface.GetAddress(0), port);
    // 5G özellikleri: Yüksek veri hızı, düşük gecikmeli paketler
    echoClient.SetAttribute("MaxPackets", UintegerValue(150));
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(2)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1472));  // Ethernet MTU
    
    ApplicationContainer clientApps = echoClient.Install(ueNodes.Get(i));
    clientApps.Start(Seconds(1.0 + i * 0.5));
    clientApps.Stop(Seconds(simTime));
    
    NS_LOG_INFO("[İSTEMCİ] UDP Echo İstemcisi " << i << " kuruldu:");
    NS_LOG_INFO("         Hedef: " << ueIpIface.GetAddress(0));
    NS_LOG_INFO("         Interval: 2ms");
    NS_LOG_INFO("         Paket Boyutu: 1472 bytes");
  }
  
  // === SİMÜLASYON ÇALIŞTIR ===
  Simulator::Stop(Seconds(simTime));
  
  NS_LOG_INFO("");
  NS_LOG_INFO("╔════════════════════════════════════════════╗");
  NS_LOG_INFO("║   SİMÜLASYON ÇALIŞIYOR...                 ║");
  NS_LOG_INFO("╚════════════════════════════════════════════╝");
  
  Simulator::Run();
  
  // === SONUÇLARI GÖSTER ===
  NS_LOG_INFO("");
  NS_LOG_INFO("╔════════════════════════════════════════════╗");
  NS_LOG_INFO("║   SİMÜLASYON SONUÇLARI                    ║");
  NS_LOG_INFO("╚════════════════════════════════════════════╝");
  NS_LOG_INFO("Simülasyon başarıyla tamamlandı!");
  NS_LOG_INFO("Simülasyon Süresi: " << simTime << " saniye");
  NS_LOG_INFO("Paket İstatistikleri için UDP Echo uygulamaları kontrol edin.");
  
  Simulator::Destroy();
  
  return 0;
}