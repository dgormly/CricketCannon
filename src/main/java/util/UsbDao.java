package util;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

/**
 * @author: Daniel Gormly
 * @date: 20/11/2016
 *
 * UsbDao is manages the communication with the Arduino via USB.
 *
 */
public class UsbDao {


    private String PORT_NAME = "IOUSBHostDevice"; // Name of IR dongle
    private int BUAD_RATE = 9600; // How fast is the baud rate of to send data at.
    private String response = "";
    private boolean setup = false;

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

        /* Setup comm port */
        comPort.openPort();
        comPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);

        /* Attach event listener to watch for incoming data. */
        comPort.addDataListener(new SerialPortDataListener() {

            @Override
            public int getListeningEvents() { return SerialPort.LISTENING_EVENT_DATA_AVAILABLE; }
            @Override
            public void serialEvent(SerialPortEvent event)
            {
                /* Something happened and it was data coming in. */
                if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) {
                    System.err.println("Event happened");
                    return;
                }

                /* Get available bytes from from the arduino. */
                byte[] newData = new byte[comPort.bytesAvailable()];
                int numRead = comPort.readBytes(newData, newData.length);
                response += new String(newData);
                if (response.contains("!")) { // End of input == '!'
                    /* Fire corresponding event */
                    switch (response) {
                        case "Setup!":
                            setup = true;
                    }

                    System.err.println(response);
                    response = "";
                }
            }
        });

        /* Sleep thread, Arduino must reset on comm connection. */
        try {
            Thread.sleep(300);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        /* System failed to successfully connect to comm port. */
        if (comPort == null || !comPort.isOpen()) {
            System.err.println("COMs failed to setup.");
        }

        if (!setup) {
            sendMessage("#Setup!");
        }

        return comPort;
    }


    /**
     * Sets whether the comm port communcation was successful.
     *
     * @param setup
     *      True if successfully setup
     *      False otherwise
     */
    public void setSetup(boolean setup) {
        this.setup = setup;
    }


    /**
     * Returns whether the comm port is setup.
     *
     * @return
     *      True if succesfully setup
     *      False otherwise
     */
    public boolean isSetup() {
        return setup;
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
