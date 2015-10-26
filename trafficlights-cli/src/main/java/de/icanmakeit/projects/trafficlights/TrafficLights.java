package de.icanmakeit.projects.trafficlights;

import de.icanmakeit.projects.trafficlights.enums.Colors;
import de.icanmakeit.projects.trafficlights.exceptions.ConnectException;
import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

import javax.annotation.Nonnull;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * Handles all the serial communication with the Arduino. This class opens a serial port with a baud rate of 57600, 8
 * databits, 1 stopbit and no flow control. The code tries to suppress the activation of the DTR line. Raising the DTR
 * signals causes most Arduinos to reset which is not desirably as it means the traffic lights will go dark with each
 * color change. Suppressing the DTR line does not work an all system (for me it worked on Windows but not on Linux).
 * If your Arduino keeps resetting, please refer to the <a href="http://playground.arduino.cc/Main/DisablingAutoResetOnSerialConnection">measures
 * listed in the Arduino playground</a>.
 *
 * @author Hendrik Busch
 * @since 2015-10-18
 */
@RequiredArgsConstructor
@Slf4j
public class TrafficLights implements SerialPortEventListener {

    /**
     * The COM port through which to connect to the Arduino.
     */
    private final String portName;

    /**
     * Timeout in milliseconds used when trying to open the COM port.
     */
    private final int connectTimeout;

    /**
     * The serial port reference that is used to actually send data to.
     */
    private SerialPort port;

    /**
     * Opens the serial port and sends the command code of the given color to the Arduino, telling it to switch to that
     * color.
     *
     * @param color
     *         the color to switch to
     */
    public void switchToColor(@Nonnull final Colors color) {
        try {
            port = openSerialPort();
            Thread.sleep(2000);
            port.writeBytes(color.getCommandCode().getBytes());
            Thread.sleep(2000);
            port.closePort();
        }
        catch (final ConnectException | SerialPortException | InterruptedException e) {
            log.error(e.getMessage());
        }
    }

    /**
     * This class is able to listen to incoming serial data from the Arduino for debugging purposes. Any data received
     * that is of type RXCHAR is printed to the output logger.
     *
     * @param serialEvent
     *         the incoming serial event
     */
    @Override
    public synchronized void serialEvent(@Nonnull final SerialPortEvent serialEvent) {
        if (serialEvent.isRXCHAR()) {
            try {
                final byte[] buf = port.readBytes(serialEvent.getEventValue());
                if (0 < buf.length) {
                    final String msg = new String(buf);
                    final List<String> rawData = new ArrayList<>(buf.length);
                    for (final byte b : buf) {
                        rawData.add("" + (int) b);
                    }
                    log.info("Incoming serial data: {} (raw: {})", msg, rawData.stream().collect(Collectors.joining(", ")));
                }
            }
            catch (final SerialPortException e) {
                log.error("Error reading incoming data: {}", e.getMessage());
            }
        }
    }

    /**
     * Opens and returns the serial port designated by {@link #portName}.
     *
     * @return the opened serial port
     *
     * @throws ConnectException
     *         if there was a problem opening the port
     */
    @Nonnull
    private SerialPort openSerialPort() throws ConnectException {
        long timeSlept = 0;
        try {
            final SerialPort port = new SerialPort(portName);
            port.openPort();
            port.setParams(SerialPort.BAUDRATE_57600, SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE, true, false);
            port.addEventListener(this);
            while (!port.isOpened()) {
                Thread.sleep(10);
                timeSlept += 10;
                if (timeSlept > connectTimeout) {
                    throw new ConnectException("Timeout while connecting to " + portName + ", connect took longer than " + connectTimeout + " milliseconds", portName);
                }
            }
            return port;
        }
        catch (InterruptedException | SerialPortException e) {
            throw new ConnectException("An error occurred while connecting to " + portName + ": " + e.getMessage(), portName, e);
        }
    }


}
