package util;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;


public class UsbDao {


    private String PORT_NAME = "IOUSBHostDevice"; // Name of IR dongle
    private int BUAD_RATE = 9600; // How fast is the baud rate of to send data at.
    private String response = "";

    private SerialPort comPort;

    /**
     * InfraredDao responsible for sending messages over infrared.
     * <p>
     * Will constantly scan for a given port until one is found using threading.
     */
    public UsbDao() {

    }

    public String getPortName() {
        return PORT_NAME;
    }


    /**
     * Returns the port the irDao is using.
     *
     * @return Port set for IR,
     * Null otherwise
     */
    public SerialPort getPort() {
        return comPort;
    }


    /**
     * Finds a port with a given description name.
     *
     * @param port
     */
    public SerialPort setPort(String port) {
        SerialPort[] ports = SerialPort.getCommPorts();

        for (SerialPort serialPort : ports) {
            if (serialPort.getDescriptivePortName().equals(port)) {
                if (comPort != null) {
                    comPort.closePort();
                }
                comPort = serialPort;
                break;
            }
        }

        comPort.openPort();
        comPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);
        comPort.addDataListener(new SerialPortDataListener() {
            @Override
            public int getListeningEvents() { return SerialPort.LISTENING_EVENT_DATA_AVAILABLE; }
            @Override
            public void serialEvent(SerialPortEvent event)
            {
                if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) {
                    System.err.println("Event happened");
                    return;
                }
                byte[] newData = new byte[comPort.bytesAvailable()];
                int numRead = comPort.readBytes(newData, newData.length);
                response += new String(newData);
                if (response.contains("!")) {
                    // Fire event and clear response.
                    System.out.println(response);
                    response = "";
                }
            }
        });

        try {
            Thread.sleep(300);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        if (comPort != null && comPort.isOpen()) {
            System.out.println("COMs setup successfully.");
        } else {
            System.out.println("COMs failed to setup.");
        }
        return comPort;
    }


    /**
     * Sends message.
     *
     * @param message Bytes to send.
     */
    public void sendMessage(String message) {
        byte[] toBytes = message.getBytes();
        if (comPort != null) {
            comPort.writeBytes(toBytes, toBytes.length);
        } else {
            System.out.println("Port == null");
        }
        System.err.println("Message sent: "+message);
    }

}
