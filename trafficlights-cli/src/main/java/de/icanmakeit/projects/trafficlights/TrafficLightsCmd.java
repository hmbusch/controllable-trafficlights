package de.icanmakeit.projects.trafficlights;

import de.icanmakeit.projects.trafficlights.enums.Colors;
import jssc.SerialPortList;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.kohsuke.args4j.CmdLineException;
import org.kohsuke.args4j.CmdLineParser;
import org.kohsuke.args4j.Option;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.StringWriter;
import java.util.Arrays;

/**
 * Command line client for controlling the traffic lights. Basic operation:
 * <br/><br/>
 * {@code TrafficLightsCmd [-lp|-lc|-h]|[-p &lt;portname&gt; &lt;color command&gt;}
 * <br/><br/>
 * <strong>Options explained</strong>
 * <ul>
 * <li>{@code -lp / --list-ports} - Show a list of available COM ports and exit</li>
 * <li>{@code -lc / --list-colors} - List all colors and exit</li>
 * <li>{@code -h / --help} - Show the command line help and exit</li>
 * <li>{@code -p &lt;portname&gt;/ --port=&lt;portname&gt;} - use the specified COM port</li>
 * <li>{@code &lt;color command&gt;} - the color to switch to (must be from the list shown by {@code -lc})
 * </ul>
 */
@Slf4j
public class TrafficLightsCmd {

    @Option(name = "-lp", usage = "List the available COM ports and exit", aliases = "--list-ports")
    private boolean listCOMPorts;

    @Option(name = "-h", usage = "Show all options and exit", aliases = "--help")
    private boolean showHelp;

    @Option(name = "-p", usage = "Use the given COM port to connect to the external device", aliases = "--port")
    private String portName;

    @Option(name = "-c", usage = "The color the traffic light should switch to", depends = "-p", aliases = "--color")
    private String colorCmd;

    @Option(name = "-lc", usage = "List all supported colors and their parameter values", aliases = "--list-colors")
    private boolean listColors;

    /**
     * Starts the program. The argument parsing and subsequent action is delegated to an instance of this class.
     *
     * @param args
     *         the given command line arguments
     */
    public static void main(final String[] args) {
        final TrafficLightsCmd trafficLightsCmd = new TrafficLightsCmd();
        trafficLightsCmd.init(args);
        trafficLightsCmd.execute();
    }

    /**
     * Controls the program flow as configured by the set flags. Listing COM ports is handled before listing colors, so
     * if both flags are specified, the latter is ignored. Any requested listing is handled before any color switching,
     * so if listing and switching are both requested, only listing is handled, switching is ignored in this
     * constellation.
     */
    private void execute() {
        if (listCOMPorts) {
            listComPortsAndExit();
        }

        if (listColors) {
            listAllColorsAndExit();
        }

        // Color and port set - the caller means business
        if (StringUtils.isNotBlank(colorCmd) && StringUtils.isNotBlank(portName)) {
            final Colors color = checkColor(colorCmd);
            final boolean portValid = checkPort(portName);

            if (null != color && portValid) {
                final TrafficLights trafficLights = new TrafficLights(portName, 2000);
                trafficLights.switchToColor(color);
            }
            else {
                log.warn("One more more required parameters are not correctly set, aborting.");
                System.exit(-1);
            }
        }
    }

    /**
     * Checks if the given port is valid. A port is considered if it appears in the port list generated by {@link
     * SerialPortList}.
     *
     * @param portName
     *         the port to check
     *
     * @return {@code true} if the port is valid, {@code false otherwise}
     */
    private boolean checkPort(@Nullable final String portName) {
        if (1 != Arrays.stream(SerialPortList.getPortNames()).filter(serialPort -> serialPort.equals(portName)).count()) {
            log.warn("Invalid port: {}", portName);
            return false;
        }
        return true;
    }

    /**
     * Checks if the given color String actually corresponds to a {@link Colors} color and return that color if it
     * does.
     *
     * @param colorCmd
     *         the color string to check
     *
     * @return the corresponding value from the {@link Colors} enum or {@code null} if the given string designates to
     * known color.
     */
    @Nullable
    private Colors checkColor(@Nonnull final String colorCmd) {
        try {
            return Colors.valueOf(colorCmd.toUpperCase());
        }
        catch (final IllegalArgumentException e) {
            log.warn("No such color: {}", colorCmd);
            return null;
        }
    }

    /**
     * Prints a list of all color names to the logger and exits with exit code 0.
     */
    private void listAllColorsAndExit() {
        log.info("List of valid color names:");
        Arrays.stream(Colors.values()).sorted((o1, o2) -> o1.name().compareToIgnoreCase(o2.name())).forEach(color -> log.info("    - {}", color.name()));
        System.exit(0);
    }

    /**
     * Prints a list of all available COM-ports to the output logger and exits with exit code 0.
     */
    private void listComPortsAndExit() {
        log.info("List of available COM ports:");
        final String[] ports = SerialPortList.getPortNames();
        for (int portNum = 0; portNum < ports.length; portNum++) {
            log.info("    Port #{}: {}", portNum, ports[portNum]);
        }
        System.exit(0);
    }


    /**
     * Initializes the instance with the give arguments. If the help is requested, the usage information is printed to
     * the output logger and the program exits with code 0. The command line will be parsed and all contained flags will
     * be assigned to the annotated instance variables.
     *
     * @param args
     *         the command line arguments to parse
     */
    private void init(@Nonnull final String[] args) {
        final CmdLineParser parser = new CmdLineParser(this);
        try {
            parser.parseArgument(args);
        }
        catch (final CmdLineException e) {
            log.error("Unable to parse command line argument: {}", e.getMessage());
            System.exit(-1);
        }

        if (showHelp) {
            final StringWriter usageWriter = new StringWriter();
            parser.printUsage(usageWriter, null);
            log.info("Usage: TrafficLightsCmd <arguments>" + System.lineSeparator() + usageWriter);
            System.exit(0);
        }
    }
}
