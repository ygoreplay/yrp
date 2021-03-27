import addon from "../build/Release/binding.node";

export function hello() {
    return addon.hello();
}
