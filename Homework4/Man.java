import java.util.Random;

public class Man implements Runnable {
    private Random r = new Random(); // Random delay generator
    private Bathroom bathroom; // Shared bathroom object
    private int numberOfVisits; // Max times this man will visit the bathroom
    public int visits = 0; // Tracks how many times he's been to the bathroom
    public int id; // Unique ID for each man

    public Man(Bathroom bathroom, int numberOfVisits, int id) { // COnstructor for assigning values
        this.bathroom = bathroom; 
        this.numberOfVisits = numberOfVisits;
        this.id = id;
    }

    public void run() {
        try {
            while (visits < numberOfVisits) {
                /* Work up to 5 seconds */
                Thread.sleep(r.nextInt(5000));

                /* Enter or wait to enter the bathroom */
                bathroom.manEnter(this);

                /* Go to the bathroom up to 2 seconds */
                Thread.sleep(r.nextInt(2000));

                /* Exit the bathroom */
                bathroom.manExit(this);

                visits++;
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            throw new RuntimeException(e);
        }
    }
}
