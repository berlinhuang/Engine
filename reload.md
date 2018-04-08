### 热更新

### 成功解决, 但是不完美

原理: 

1. 热更前, 先缓存旧模块数据, 重置加载标记标记.

    local oldmodule = _G[ mname ]
    	_G[ mname ] = nil
    	package.loaded[mname]=nil

2. require 新模块

   - 需要是用pcall, 不会在错误时, 直接退出执行体.
   - 出错时, 需要把oldmodule还原回来.
   - 没有还原会出什么问题呢? (新的调用不到? 旧的呢? 为何当时改变self写法后, 就可以没事, 后续还能热更吗? 再也热更不了了吗?)

   ```
   local bSucess,strErr = pcall( require, mname ) 
   if not bSucess then
   	_G[ mname ] = oldmodule
   	package.loaded[mname] = oldmodule
   	ScriptSys.log( 0 , "reload fail: " .. strErr )
   	return
   end
   ```

3.  热更逻辑, 保留数据. 也就是还原环境. | 修正  newmodule中, upvalue 部分

   - 数据类型直接还原. 

     ```lua
     debug.setupvalue(env_f, i, old_value)
     ```


   - newmodule中upvalue为table, 替换oldmodule中的table.  

     - ?


     - table中 pairs可以遍历的, key, val 全部替换为old_value[key].
     - function 需要递归处理upvalues.

   - new

4. ?

```lua
-- old_function key with new function_value
-- new funtion_value with old table_data
function hotfix(oldmodule, newmodule) -- 核心热更函数
    local protection = {
        setmetatable = true,
        pairs = true,
        ipairs = true,
        next = true,
        require = true,
        _ENV = true,
    }
    --防止重复的table替换，造成死循环
    local visited_sig = {}
    -- 这边需要用新table中变更的部分, 替换进旧table中
  	-- 实际更改部分:1.函数地址; 2.key相同, 但类型发生变化的. 
    -- 注意,热更时, newmodule使用旧的table地址??
    -- 第二点要注意, 为了支持table结构热更修改, 优先考虑新模块的类型, 比如加字段.
    -- 可能造成,在运行中动态赋值部分, 如果类型不同会被覆盖(新值nil不会覆盖). 
    -- 建议一个变量, 别指向多种类型.
    function update_table(env_t, g_t, name, deep)
    	--对某些关键函数不进行比对
        if protection[env_t] or protection[g_t] then return end
    	--如果原值与当前值内存一致，值一样不进行对比. 一个table, 可以是多个函数的upvalue
        if env_t == g_t then return end
        local signature = tostring(g_t)..tostring(env_t)
        if visited_sig[signature] then return end
        visited_sig[signature] = true
    	--遍历新表 对比值，如进行遍历env类似的步骤
        for name, value in pairs(env_t) do
            local old_value = g_t[name]
            if type(value) == type(old_value) then --类型不变
                if type(value) == 'function' then
                    update_func(value, old_value, name, deep..'  '..name..'  ')
          			-- function地址变了, 要换新表里面的
                    g_t[name] = value 
                elseif type(value) == 'table' then
                    update_table(value, old_value, name, deep..'  '..name..'  ')
          			-- 注意, 这里table地址没变
                end
            else -- 类型变化,直接更新
                g_t[name] = value
            end
        end
    	--遍历table的元表，进行对比
    	--何时需要变动old_table的metatable? 
    	--可能是加载的时候,在函数外面执行的语句, 修改了local table的metatable,
        --这边作为函数的upvalue,得到更新.
        local old_meta = debug.getmetatable(g_t)
        local new_meta = debug.getmetatable(env_t)
        if type(old_meta) == 'table' and type(new_meta) == 'table' then
            update_table(new_meta, old_meta, name..'s Meta', deep..'  '..name..'s Meta'..'  ' )
        end
    end
    -- 通过env当前的值和_G当前的上值进行对比
    -- 1.如果类型不同我们直接覆盖原值，此时value不为nil，不会出现原值被覆盖成nil的情况
    -- 2.如果当前值为函数，我们进行函数的upvalue值比对
  	-- 热更后, 使用的function是newmodule_function, upvalue是_oldmodule_function的函数的upvalue 
    -- 即,新的逻辑, 新的数据(运行过程中最近的数据)
    function update_func(env_f, g_f, name, deep) -- newmodule_function _oldmodule_function
        -- signature +visited_sig[] 避免递归
        local signature = tostring(env_f)..tostring(g_f)
        if visited_sig[signature] then return end
        visited_sig[signature] = true
        local old_upvalue_map = {}
        -- 读出所有_oldmodule_function的upvalue值
        for i = 1, math.huge do
            local name, value = debug.getupvalue(g_f, i)
            if not name then break end
            old_upvalue_map[name] = value
        end
    	--遍历_newmodule_function中存在的upvalue(初始值),
    	--使用_oldmodule_function中当前值, 替换到 _newmodule_function
        for i = 1, math.huge do
            local name, value = debug.getupvalue(env_f, i) 
            if not name then break end
            local old_value = old_upvalue_map[name]
            if old_value then-- 把旧的upvalue转移过来
                if type(old_value) ~= type(value) then
                    debug.setupvalue(env_f, i, old_value) -- 用 old_value 替换
                elseif type(old_value) == 'function' then
                    update_func(value, old_value, name, deep..'  '..name..'  ') -- 递归更新
                elseif type(old_value) == 'table' then
          			-- oldtable更新完后, set to _newmodule_function
                    update_table(value, old_value, name, deep..'  '..name..'  ')
                    debug.setupvalue(env_f, i, old_value) 
                else -- 其他普通类型,直接使用old_value
                    debug.setupvalue(env_f, i, old_value)
                end
            end
        end
    end
  
    for name,value in pairs(newmodule) do
        local old_value = oldmodule[name]
        if type(old_value ) ~= type(value) then
      		-- 类型变更的新值为准,新值非空,不会因热更导致正常值变为nil
            --_G[name] = value -- 这里似乎应该改为oldmodule
      		oldmodule[name] = value
        elseif type(value) == 'function' then
            update_func(value, old_value , name, 'oldmodule'..'  ')
   			-- 这边非local 函数地址发生变化.(保存函数地址的写法,需要注意)
            --_G[name] = value
      		oldmodule[name] = value 
        elseif type(value) == 'table' then
            update_table(value, old_value , name, 'oldmodule'..'  ')
        end
    end
    return 0
end
function reload( mname )
	if type(mname) ~= "string" then
		ScriptSys.log( 0 , "module name invalid." )
		return
	end
	local oldmodule = _G[ mname ]
	_G[ mname ] = nil
	package.loaded[mname]=nil
	local bSucess,strErr = pcall( require, mname )
	if not bSucess then
		_G[ mname ] = oldmodule
		package.loaded[mname] = oldmodule
		ScriptSys.log( 0 , "reload fail: " .. strErr )
		return
	end
	local newmodule = _G[ mname ]
    print("new module:")
    printModule(newmodule)

	if oldmodule then
        hotfix(oldmodule, newmodule)
		oldmodule._M = oldmodule
		_G[ mname ] = oldmodule
		package.loaded[ mname ] = oldmodule
    end	

	ScriptSys.log( 0 , "reload module " .. mname .. " successfull" )
	return true
end

```

- 可能会疑惑那些new出去的对象怎么保证得到修改?

  newObj 自身的成员table,访问没有变动.

  newObj 通过元表调用方法. 而oldmodule地址没变, 但里面的函数地址已经更新

    `newObj.Foo` `newObj:Foo`都能找到新的地址. 

  > 注意, 尽量采用这种方式调用, 若是保存的方式, 就只能按地址调用.
  >
  > local lcFoo = newObj.Foo
  >
  > lcFoo() -- 这种写法, 存了老版本的地址, 最终会调用老式的函数体, 热更不到.

  ?

  ?


- 看热更遇到的最大疑惑是, module里面的local部分

  local variable, local function 我都无法使用 pairs遍历出来.  此时知道对function可以有up value 的更新.

  - 可以通过`debug.getupvalue` `debug.getlocal(f, i)`来获取函数的up value, 和local value
  - 为此特地补了module, 环境, closure 甚至Lua词法分析过程等相关知识. 没有找到答案.
  - [LUA外部模块加载机制](http://www.codedump.info/?p=402)
  - [词法分析 虚拟机概述](http://www.codedump.info/?p=310)
  - [Lua栈](http://www.codedump.info/?p=335)

  ### 什么是function 的up value呢?

  - [Lua热更新(hotfix)](http://blog.csdn.net/jq0123/article/details/51819588)我在这里找到答案

  - >例如 test.lua:
    >local M = {}
    >local a = "old"
    >function M.get_a() return a end
    >return M
    >
    >更新到
    >local M = {}
    >local a = "new"
    >function M.get_a() return a .. "_x" end
    >return M
    >
    >运行 
    >local hotfix = require("hotfix")
    >local test = hotfix.hotfix_module("test")
    >test.get_a()  -- "old_x"
    >
    >**数据 a 作为函数的upvalue得到了保留，但是函数得到了更新**。

    文件作用域里面的local a 作为函数的upvalue 得到保留. 这个对我而言,豁然开朗 地解决掉前述的问题.

    ?

### 下一步 

- 需要在lua 5.1中, 解决 module和非module两种类型的,lua模块的热更.

  - 能否使用require 直接加载脚本呢?

  - 是否需要对两种写法的文件做识别?

  - 项目中的reload, 新模块的加载, 直接更新了local function 和local variable 的流程还尚未完全清晰. 

    ?

##### lua加密后, 热更失效的情况

http://blog.csdn.net/x2345com/article/details/53510498

##### 热更面试题

http://blog.csdn.net/qq_32319583/article/details/53223452

##### skynet控制台使用技巧 

http://blog.csdn.net/mycwq/article/details/50472692

http://blog.csdn.net/ad88282284/article/details/43994103

##### 5.2以上的链接暂存

http://blog.csdn.net/cbbbc/article/details/45155135

