package util;

import com.fazecast.jSerialComm.SerialPort;

import java.nio.charset.StandardCharsets;

public class InfraredDao {


    private String PORT_NAME = "UMFT234XF"; // Name of IR dongle
    private int SEND_NUM = 1; // Number of times to send data over IR
    private int BUAD_RATE = 1200; // How fast is the baud rate of to send data at.

    private SerialPort comPort = null;

    /**
     * InfraredDao responsible for sending messages over infrared.
     *
     * Will constantly scan for a given port until one is found using threading.
     */
    public InfraredDao() {
    }

    public String getPortName() {
        return PORT_NAME;
    }


    /**
     * Returns the port the irDao is using.
     *
     * @return
     *      Port set for IR,
     *      Null otherwise
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
                comPort = serialPort;
                break;
            }
        }

        System.out.println("COMs setup successfully.");
        return comPort;
    }


    /**
     * Sends message.
     *
     * @param message
     *      Bytes to send.
     */
    public void sendMessage(String message) {
        byte[] toBytes = message.getBytes(StandardCharsets.UTF_8);

        for (int i = 0; i < SEND_NUM; i++) {
            if (comPort != null) {
                comPort.writeBytes(toBytes, toBytes.length);
            } else {
                System.out.println("Port == null");
            }
        }


        comPort.openPort();
        comPort.setBaudRate(BUAD_RATE);
        comPort.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 100, 100);
        System.out.println("Message sent: " + message);
        System.out.println(comPort.openPort());
        System.out.println(comPort.getDescriptivePortName());
    }



}
