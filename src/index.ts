import addon from "../build/Release/binding.node";

export function hello() {
    return addon.lzmaUncompress(3, 2);
}
