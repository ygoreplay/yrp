import { uncompressReplayData } from ".";
import * as fs from "fs";
import path from "path";

describe("yrp-parser-addon", () => {
    it("should uncompress replay data properly", () => {
        const originalFileBuffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));

        expect(() => {
            uncompressReplayData(originalFileBuffer);
        }).not.toThrow();
    });
});
