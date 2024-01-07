#include "socket.h"

CSOcket::CSOcket(std::string _host, std::function<void(std::string, CSOcket *)> messageH)
{
  onmessage = messageH;
  host = _host;
}
void CSOcket::connect()
{
  cli = WiFiClient();
  cli.connect(host.c_str(), port, 30000);
  connection = true;
}
void CSOcket::step()
{
  if (!connection)
  {
    return;
  }

  size_t available = cli.available();

  bool wasAvailable = false;
  uint8_t last = 0;
  while (available)
  {
    uint8_t _char = cli.read();

    if (_char == '\n' && last == '\n')
    {
      std::string strVal;
      receiveBuffer >> strVal;
      receiveBuffer.clear();
      onmessage(strVal, this);
      return;
    }
    last = _char;
    receiveBuffer << _char;
    available = cli.available();
  }
}

void CSOcket::write(std::string data)
{

  if (!cli.connected())
  {
    Serial.println("not connected");
  }
  int err = cli.getWriteError();
  for (std::string::size_type i = 0; i < data.size(); i++)
  {

    cli.write(data[i]);
  }
  cli.write('\n');
  cli.write('\n');
}