package de.icanmakeit.projects.trafficlights.exceptions;

import lombok.Getter;

import javax.annotation.Nullable;

/**
 * Signals that there was a problem connecting to a COM port.
 *
 * @author Hendrik Busch
 * @since 2015-10-26
 */
public class ConnectException extends Exception {

    /**
     * The name of the port that failed to open.
     */
    @Nullable
    @Getter
    private final String portName;

    /**
     * Creates a new instance with a message detailing the problem and the name of the port that failed to open.
     *
     * @param message
     *         the message
     * @param portName
     *         the portname
     */
    public ConnectException(@Nullable final String message, @Nullable final String portName) {
        super(message);
        this.portName = portName;
    }

    /**
     * Creates a new instance with a message detailing the problem, the name of the port that failed to open and a cause
     * for this exception.
     *
     * @param message
     *         the message
     * @param portName
     *         the portname
     * @param cause
     *         another exception that caused this {@code ConnectException}
     */
    public ConnectException(@Nullable final String message, @Nullable final String portName, @Nullable final Throwable cause) {
        super(message, cause);
        this.portName = portName;
    }
}
