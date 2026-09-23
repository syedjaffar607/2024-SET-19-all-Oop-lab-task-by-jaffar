#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>
#include <string>

using namespace std;

// =========================
// Base Class: Vehicle
// =========================
class Vehicle {
private:
    string model;
    double dailyRate;
    bool available;

public:
    Vehicle(const string& model, double dailyRate)
        : model(model), dailyRate(dailyRate), available(true) {}

    virtual ~Vehicle() = default;

    string getModel() const {
        return model;
    }

    double getDailyRate() const {
        return dailyRate;
    }

    bool isAvailable() const {
        return available;
    }

    void setAvailability(bool status) {
        available = status;
    }

    virtual double calculateCost(int days) const {
        return dailyRate * days;
    }

    virtual string getType() const {
        return "Vehicle";
    }

    virtual void displayInfo() const {
        cout << getType() << " - " << model
             << " | Rate: $" << fixed << setprecision(2) << dailyRate
             << " | Status: "
             << (available ? "Available" : "Rented") << '\n';
    }
};

// =========================
// Derived Class: Car
// =========================
class Car : public Vehicle {
private:
    int seats;

public:
    Car(const string& model, double dailyRate, int seats)
        : Vehicle(model, dailyRate), seats(seats) {}

    string getType() const override {
        return "Car";
    }

    void displayInfo() const override {
        Vehicle::displayInfo();
        cout << "  Seats: " << seats << '\n';
    }
};

// =========================
// Derived Class: Motorbike
// =========================
class Motorbike : public Vehicle {
private:
    int engineCC;

public:
    Motorbike(const string& model, double dailyRate, int engineCC)
        : Vehicle(model, dailyRate), engineCC(engineCC) {}

    string getType() const override {
        return "Motorbike";
    }

    double calculateCost(int days) const override {
        double cost = getDailyRate() * days;

        // 10% discount for rentals longer than 7 days
        if (days > 7) {
            cost *= 0.90;
        }

        return cost;
    }

    void displayInfo() const override {
        Vehicle::displayInfo();
        cout << "  Engine: " << engineCC << " CC\n";
    }
};

// =========================
// Derived Class: Truck
// =========================
class Truck : public Vehicle {
private:
    double payloadCapacity;

public:
    Truck(const string& model, double dailyRate, double payloadCapacity)
        : Vehicle(model, dailyRate), payloadCapacity(payloadCapacity) {}

    string getType() const override {
        return "Truck";
    }

    double calculateCost(int days) const override {
        double cost = getDailyRate() * days;

        // 20% surcharge for trucks
        cost *= 1.20;

        return cost;
    }

    void displayInfo() const override {
        Vehicle::displayInfo();
        cout << "  Payload: " << payloadCapacity << " tonnes\n";
    }
};

// =========================
// Customer Class
// =========================
class Customer {
private:
    int id;
    string name;

public:
    Customer(int id, const string& name)
        : id(id), name(name) {}

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }
};

// =========================
// Rental Class
// =========================
class Rental {
private:
    int customerId;
    int vehicleId;
    int days;
    double cost;
    bool active;

public:
    Rental(int customerId, int vehicleId, int days, double cost)
        : customerId(customerId),
          vehicleId(vehicleId),
          days(days),
          cost(cost),
          active(true) {}

    int getCustomerId() const {
        return customerId;
    }

    int getVehicleId() const {
        return vehicleId;
    }

    bool isActive() const {
        return active;
    }

    void closeRental() {
        active = false;
    }

    void display(const string& customerName,
                 const string& vehicleModel) const {
        cout << "Customer: " << customerName
             << " | Vehicle: " << vehicleModel
             << " | Days: " << days
             << " | Cost: $" << fixed << setprecision(2) << cost
             << " | Status: " << (active ? "Active" : "Closed")
             << '\n';
    }
};

// =========================
// RentalSystem Class
// =========================
class RentalSystem {
private:
    vector<unique_ptr<Vehicle>> vehicles;
    vector<Customer> customers;
    vector<Rental> rentals;

    Vehicle* findVehicle(int vehicleId) {
        if (vehicleId < 0 ||
            vehicleId >= static_cast<int>(vehicles.size())) {
            return nullptr;
        }

        return vehicles[vehicleId].get();
    }

    const Customer* findCustomer(int customerId) const {
        for (const auto& customer : customers) {
            if (customer.getId() == customerId) {
                return &customer;
            }
        }
        return nullptr;
    }

    bool hasActiveRental(int customerId) const {
        for (const auto& rental : rentals) {
            if (rental.isActive() &&
                rental.getCustomerId() == customerId) {
                return true;
            }
        }
        return false;
    }

public:
    void addVehicle(unique_ptr<Vehicle> vehicle) {
        vehicles.push_back(move(vehicle));
    }

    void registerCustomer(const Customer& customer) {
        customers.push_back(customer);
    }

    bool rentVehicle(int customerId, int vehicleId, int days) {
        const Customer* customer = findCustomer(customerId);
        Vehicle* vehicle = findVehicle(vehicleId);

        if (customer == nullptr) {
            cout << "Rental failed: Customer not found.\n";
            return false;
        }

        if (vehicle == nullptr) {
            cout << "Rental failed: Vehicle not found.\n";
            return false;
        }

        if (days <= 0) {
            cout << "Rental failed: Days must be greater than 0.\n";
            return false;
        }

        if (!vehicle->isAvailable()) {
            cout << "Rental failed: Vehicle is already rented.\n";
            return false;
        }

        if (hasActiveRental(customerId)) {
            cout << "Rental failed: Customer already has an active rental.\n";
            return false;
        }

        // Runtime polymorphism happens here.
        double cost = vehicle->calculateCost(days);

        rentals.emplace_back(customerId, vehicleId, days, cost);
        vehicle->setAvailability(false);

        cout << "Rental successful: "
             << customer->getName() << " rented "
             << vehicle->getModel()
             << " for " << days << " day(s).\n";

        cout << "Total cost: $"
             << fixed << setprecision(2) << cost << "\n\n";

        return true;
    }

    bool returnVehicle(int vehicleId) {
        Vehicle* vehicle = findVehicle(vehicleId);

        if (vehicle == nullptr) {
            cout << "Return failed: Vehicle not found.\n";
            return false;
        }

        if (vehicle->isAvailable()) {
            cout << "Return failed: Vehicle is not currently rented.\n";
            return false;
        }

        for (auto& rental : rentals) {
            if (rental.isActive() &&
                rental.getVehicleId() == vehicleId) {

                rental.closeRental();
                vehicle->setAvailability(true);

                cout << "Vehicle returned successfully: "
                     << vehicle->getModel() << "\n\n";

                return true;
            }
        }

        cout << "Return failed: Active rental record not found.\n";
        return false;
    }

    void printSummary() const {
        int availableCount = 0;
        int rentedCount = 0;

        for (const auto& vehicle : vehicles) {
            if (vehicle->isAvailable()) {
                ++availableCount;
            } else {
                ++rentedCount;
            }
        }

        cout << "\n========================================\n";
        cout << "       VEHICLE RENTAL SUMMARY\n";
        cout << "========================================\n";

        cout << "\nCurrent Active Rentals:\n";

        bool foundActiveRental = false;

        for (const auto& rental : rentals) {
            if (rental.isActive()) {
                const Customer* customer =
                    findCustomer(rental.getCustomerId());

                const Vehicle* vehicle =
                    vehicles[rental.getVehicleId()].get();

                if (customer != nullptr && vehicle != nullptr) {
                    rental.display(customer->getName(),
                                   vehicle->getModel());
                    foundActiveRental = true;
                }
            }
        }

        if (!foundActiveRental) {
            cout << "No active rentals.\n";
        }

        cout << "\nVehicle Status:\n";
        cout << "Available: " << availableCount << '\n';
        cout << "Rented: " << rentedCount << '\n';

        cout << "\nAll Vehicles:\n";
        for (const auto& vehicle : vehicles) {
            vehicle->displayInfo();
        }

        cout << "========================================\n";
    }
};

// =========================
// Main Function
// =========================
int main() {
    RentalSystem system;

    // 1. Add at least 3 vehicles
    system.addVehicle(
        make_unique<Car>("Toyota Corolla", 50.0, 5)
    );

    system.addVehicle(
        make_unique<Motorbike>("Honda CB 150F", 30.0, 150)
    );

    system.addVehicle(
        make_unique<Truck>("Isuzu Truck", 100.0, 5.0)
    );

    // 2. Register 2 customers
    system.registerCustomer(Customer(101, "Ali"));
    system.registerCustomer(Customer(102, "Ahmed"));

    // 3. Process 2 rentals
    system.rentVehicle(101, 0, 3);  // Ali rents car
    system.rentVehicle(102, 2, 5);  // Ahmed rents truck

    // 4. Return 1 vehicle
    system.returnVehicle(0);

    // 5. Print summary
    system.printSummary();

    return 0;
}
