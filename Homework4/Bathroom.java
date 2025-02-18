import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

public class Bathroom {
    private int currentMen = 0;
    private int currentWomen = 0;
    private int waitingMen = 0;
    private int waitingWomen = 0;
    private final DateTimeFormatter timeFormat = DateTimeFormatter.ofPattern("HH:mm:ss:SSS");

    public synchronized void manEnter(Man man) throws InterruptedException {
        waitingMen++;
        logEvent("Man " + man.id + " wants to enter.");
        while (currentWomen > 0 || currentMen > 0) { // If women are inside wait
            wait();
        }
        waitingMen--;
        currentMen = 1;
        logEvent("Man " + man.id + " enters the bathroom. ");
    }

    public synchronized void manExit(Man man) {
        currentMen--;
        logEvent("Man " + man.id + " exits the bathroom.");

        if (currentMen == 0 && waitingWomen > 0) {
            notifyAll(); // wake up waiting women
        }
    }

    public synchronized void womanEnter(Woman woman) throws InterruptedException {
        waitingWomen++;
        logEvent("Woman " + woman.id + " wants to enter.");
        while (currentMen > 0 || currentWomen > 0) {
            wait();
        }
        waitingWomen--;
        currentWomen = 1;
        logEvent("Woman " + woman.id + " enters the bathroom.");
    }

    public synchronized void womanExit(Woman woman) {
        currentWomen--;
        logEvent("Woman " + woman.id + " exits the bathroom.");

        if (currentWomen == 0 && waitingMen > 0) {
            notifyAll();
        }
    }

    private void logEvent(String message) {
        System.out.println(LocalTime.now().format(timeFormat) + " - " + message);
    }

}