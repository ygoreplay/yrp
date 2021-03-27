import { hello } from ".";

describe("yrp-parser-node", () => {
    it("should print world", () => {
        expect(hello()).toBe("world");
    });
});
