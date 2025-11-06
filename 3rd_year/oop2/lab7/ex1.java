package lab7;
//package lab7;
import java.util.Scanner;

public class ex1 {
    
    public static int readInteger() {
        Scanner scanner = new Scanner(System.in);
        int number = 0;
        boolean valid = false;
        
        try {
            System.out.print("Introduceți un număr întreg: ");
            String input = scanner.nextLine();
            number = Integer.parseInt(input);
            valid = true;
            System.out.println("Conversie realizată cu succes.");
        } catch (NumberFormatException e) {
            System.out.println("Eroare: Valoarea introdusă nu este un număr întreg valid.");
            System.out.println("Detalii excepție: " + e.getMessage());
        } finally {
            System.out.println("Blocul finally se execută întotdeauna, indiferent dacă a apărut o excepție sau nu.");
            if (valid) {
                System.out.println("S-a citit numărul: " + number);
            } else {
                System.out.println("Nu s-a putut citi un număr valid.");
            }
        }
        
        return number;
    }
    
    public static void main(String[] args) {
        System.out.println("Demonstrație pentru try-catch-finally");
        int result = readInteger();
        System.out.println("Programul continuă cu valoarea: " + result);
    }
}