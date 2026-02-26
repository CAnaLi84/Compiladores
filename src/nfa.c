/*Programa para modelar pasar de una expresión regular escrita de forma
  infija de concatenación implicita a un autómata finito no determinista.
*/



typedef struct Transition {
    char symbol;              //Símbolo de transición. Epsilon es el caracter '/e'
    struct State* to;         //Estado de llegada
    struct transicion* next;  //lsita de estados
} Transition;


/*Estructura para modelar estados*/
typedef struct State{
    int id;
    bool is_final;
    Transition* transitions;
}State;

