#include <iostream>
#include <vector>
#include <string>

using namespace std;


// ---- Pizza abstracta ----
class Pizza {
public:
    virtual string getName() const = 0;
    virtual double getPrice() const = 0;
};

// ---- Interfaz para Observer ----
class Observer {
public:
    virtual void update(double total) = 0;
};

// ---- Interfaz para pagos ----
class Payment {
public:
    virtual void pay(double amount) = 0;
};

// ============ CONCRETAS DE PIZZA ===============

// Pizza personalizada (usa Builder)
class CustomPizza : public Pizza {
    string name;
    double price;

public:
    CustomPizza(string n, double p) : name(n), price(p) {}

    string getName() const override { return name; }
    double getPrice() const override { return price; }
};

// Tipos de pizza concretos
class PepperoniPizza : public Pizza {
public:
    string getName() const override { return "Pizza Pepperoni"; }
    double getPrice() const override { return 40; }
};

class HawaiianPizza : public Pizza {
public:
    string getName() const override { return "Pizza Hawaiana"; }
    double getPrice() const override { return 50; }
};

// ============ BUILDER DE PIZZA PERSONALIZADA ===============

class PizzaBuilder {
    string name = "Pizza Personalizada";
    double price = 0;

public:
    PizzaBuilder& addCheese() {
        price += 10;
        return *this;
    }

    PizzaBuilder& addPepperoni() {
        price += 12;
        return *this;
    }

    PizzaBuilder& addPineapple() {
        price += 8;
        return *this;
    }

    Pizza* build() {
        return new CustomPizza(name, price);
    }
};

// ============ PAGOS ===============

class CashPayment : public Payment {
public:
    void pay(double amount) override {
        cout << "Pagando Bs" << amount << " en efectivo.\n";
    }
};

class CardPayment : public Payment {
public:
    void pay(double amount) override {
        cout << "Pagando Bs" << amount << " con tarjeta.\n";
    }
};

// ============ ADAPTER ===============

// API externa simulada (incompatible con Payment)
class ExternalPaymentAPI {
public:
    void doTransaction(double amount) {
        cout << "Pago realizado mediante API externa: Bs" << amount << endl;
    }
};

// Adaptador que convierte ExternalPaymentAPI en Payment
class ExternalPaymentAdapter : public Payment {
    ExternalPaymentAPI* api;

public:
    ExternalPaymentAdapter(ExternalPaymentAPI* a) : api(a) {}
    void pay(double amount) override {
        api->doTransaction(amount);
    }
};

// ============ OBSERVER CONCRETOS ===============

class EmailNotifier : public Observer {
public:
    void update(double total) override {
        cout << "[Email] Enviando confirmación de pedido por Bs" << total << "...\n";
    }
};

class AuditLogger : public Observer {
public:
    void update(double total) override {
        cout << "[Log] Pedido registrado por Bs" << total << ".\n";
    }
};

// ============ ORDER (SUJETO OBSERVABLE) ===============

class Order {
    vector<Pizza*> pizzas;
    vector<Observer*> observers;

public:
    void addPizza(Pizza* pizza) {
        pizzas.push_back(pizza);
    }

    void addObserver(Observer* obs) {
        observers.push_back(obs);
    }

    void notifyObservers(double total) {
        for (auto obs : observers) {
            obs->update(total);
        }
    }

    void listOrder() const {
        cout << "Pizzas en el pedido:\n";
        for (const auto& pizza : pizzas) {
            cout << "- " << pizza->getName() << " (Bs" << pizza->getPrice() << ")\n";
        }
    }

    double calculateTotal() const {
        double total = 0;
        for (const auto& pizza : pizzas) {
            total += pizza->getPrice();
        }
        return total;
    }

    void checkout(Payment* paymentMethod) {
        double total = calculateTotal();
        notifyObservers(total);
        cout << "Total a pagar: Bs" << total << endl;
        paymentMethod->pay(total);
    }

    void clearOrder() {
        for (auto pizza : pizzas) {
            delete pizza;
        }
        pizzas.clear();
    }
};

// ======================== MAIN ===========================

int main() {
    Order order;

    // Agregar observadores
    order.addObserver(new EmailNotifier());
    order.addObserver(new AuditLogger());

    int option;

    do {
        cout << "\n=== Menu de Pizzas ===\n";
        cout << "1. Pizza Pepperoni (Bs 40)\n";
        cout << "2. Pizza Hawaiana (Bs 50)\n";
        cout << "3. Pizza Personalizada\n";
        cout << "4. Finalizar pedido\n";
        cout << "Seleccione una opcion: ";
        cin >> option;

        switch (option) {
            case 1:
                order.addPizza(new PepperoniPizza());
                break;
            case 2:
                order.addPizza(new HawaiianPizza());
                break;
            case 3: {
                PizzaBuilder builder;
                int ingrediente;

                cout << "Elija ingredientes para su pizza personalizada:\n";
                cout << "1. Queso (+Bs10)\n";
                cout << "2. Pepperoni (+Bs12)\n";
                cout << "3. Piña (+Bs8)\n";
                cout << "0. Terminar\n";

                do {
                    cout << "Ingrediente: ";
                    cin >> ingrediente;
                    switch (ingrediente) {
                        case 1: builder.addCheese(); break;
                        case 2: builder.addPepperoni(); break;
                        case 3: builder.addPineapple(); break;
                        case 0: break;
                        default: cout << "Opción no válida.\n";
                    }
                } while (ingrediente != 0);

                order.addPizza(builder.build());
                break;
            }
            case 4:
                cout << "Finalizando pedido...\n";
                break;
            default:
                cout << "Opción no válida.\n";
        }

    } while (option != 4);

    order.listOrder();

    cout << "\nSeleccione metodo de pago:\n";
    cout << "1. Efectivo\n";
    cout << "2. Tarjeta\n";
    cout << "3. API Externa (Adapter)\n";
    int paymentOption;
    cin >> paymentOption;

    Payment* payment;
    if (paymentOption == 1) {
        payment = new CashPayment();
    } else if (paymentOption == 2) {
        payment = new CardPayment();
    } else if (paymentOption == 3) {
        payment = new ExternalPaymentAdapter(new ExternalPaymentAPI());
    } else {
        cout << "Método inválido. Se usará efectivo por defecto.\n";
        payment = new CashPayment();
    }

    order.checkout(payment);

    delete payment;
    order.clearOrder();

    return 0;
}

