public class BathroomSimulation {
    public static void main(String[] args) {
        int numMen = 5;
        int numWomen = 5;
        int numVisits = 3;

        Bathroom bathroom = new Bathroom();

        Thread[] menThreads = new Thread[numMen];
        Thread[] womenThreads = new Thread[numWomen];

        for (int i = 0; i < numMen; i++) {
            menThreads[i] = new Thread(new Man(bathroom, numVisits, i + 1));
            menThreads[i].start();
        }

        for (int i = 0; i < numWomen; i++) {
            womenThreads[i] = new Thread(new Woman(bathroom, numVisits, i + 1));
            womenThreads[i].start();
        }

        // Wait for all threads to finish
        try {
            for (Thread t : menThreads) t.join();
            for (Thread t : womenThreads) t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("Simulation finished.");
    }
}

