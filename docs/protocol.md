# Communication Protocol
## Framing
UMN uses framing to transmit data between nodes.
The structure of a frame is as follow:
<dl>
<table>
  <tr>
    <th>START_SYMBOL (8 bits)</th>
    <th>FLAGS (8 bits)</th>
    <th>FRAME_TYPE (8 bits)</th>
    <th>SENDER_NODE_ID (16 bits)</th>
    <th>DESTINATION_NODE_ID (16 bits)</th>
    <th>PAYLOAD SIZE (8 bit)</th>
    <th>PAYLOAD (Variable Size, Max=255bytes)</th>
    <th>Checksum<br>(16 bits)</th>
    <th>END_SYMBOL (8 bits)</th>
  </tr>
  <tr>
    <td>0xBE</td>
    <td>Can be used to specify<br>frame encoding/encryption options<br>and stuff</td>
    <td>The kind of packet<br>the frame carries<br>(please refer to the "Packets" chapter<br>of this document)</td>
    <td></td>
    <td></td>
    <td>Size in bytes of the<br>payload. So, indeed, maximum payload size is 255.</td>
    <td>Payload content</td>
    <td>CCITT CRC 16 (poly: 0x1021, start=0x00)<br>Computed on the following elements:<br>FRAME_TYPE, SENDER_NODE_ID, DESTINATION_NODE_ID, PAYLOAD_SIZE, PAYLOAD</td>
    <td>0xEF</td>
  </tr>
</table>
</dl>
