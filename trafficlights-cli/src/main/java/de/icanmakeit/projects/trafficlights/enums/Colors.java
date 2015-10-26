package de.icanmakeit.projects.trafficlights.enums;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

import javax.annotation.Nonnull;

/**
 * Enumeration of the supported colors. This list must match the one in the Arduino sketch for the software to work
 * properly.
 *
 * @author Hendrik Busch
 * @since 2015-10-18
 */
@RequiredArgsConstructor
public enum Colors {

    /**
     * Black actually means off.
     */
    BLACK("o"),
    RED("r"),
    YELLOW("y"),
    GREEN("g"),
    BLUE("b"),
    WHITE("w"),

    /**
     * Test is not a real color, it instead runs a small test routine on the controller.
     */
    TEST("t");

    /**
     * The command code that will be sent to the Arduino. Must be a single character if you use my Arduino sketch.
     */
    @Getter
    @Nonnull
    private final String commandCode;

}
