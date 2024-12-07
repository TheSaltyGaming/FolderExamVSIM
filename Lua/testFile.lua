print("Hello from Lua!")

function myLuaFunction(a, b)
    local sum = a + b
    print("The sum of " .. a .. " and " .. b .. " is: " .. sum)
    return sum * 2  -- Return a value back to C++
end

PrintText("Hello from C++!")