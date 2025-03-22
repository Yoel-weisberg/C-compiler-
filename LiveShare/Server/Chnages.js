export default class Change {
    constructor(location, amount) {
        this.location = location;
        this.amount = amount;
    }

    toJson() {
        return {location: this.location, amount: this.amount};
    }
}