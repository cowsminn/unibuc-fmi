package poo.database;

import java.util.List;

/**
 * Interface generică pentru operațiunile CRUD (Create, Read, Update, Delete)
 * @param <T> Tipul obiectului cu care lucrează DAO-ul
 * @param <ID> Tipul ID-ului (Integer pentru noua schemă)
 */
public interface GenericDAO<T, ID> {
    
    /**
     * Creează o nouă înregistrare în baza de date
     * @param entity Obiectul de creat
     * @return ID-ul obiectului creat sau null dacă operațiunea a eșuat
     */
    ID create(T entity);
    
    /**
     * Citește o înregistrare după ID
     * @param id ID-ul înregistrării
     * @return Obiectul găsit sau null dacă nu există
     */
    T read(ID id);
    
    /**
     * Actualizează o înregistrare existentă
     * @param entity Obiectul cu datele actualizate
     * @return true dacă operațiunea a reușit
     */
    boolean update(T entity);
    
    /**
     * Șterge o înregistrare după ID
     * @param id ID-ul înregistrării de șters
     * @return true dacă operațiunea a reușit
     */
    boolean delete(ID id);
    
    /**
     * Returnează toate înregistrările
     * @return Lista cu toate obiectele
     */
    List<T> getAll();
}