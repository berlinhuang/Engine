### �ȸ���

### �ɹ����, ���ǲ�����

ԭ��: 

1. �ȸ�ǰ, �Ȼ����ģ������, ���ü��ر�Ǳ��.

    local oldmodule = _G[ mname ]
    	_G[ mname ] = nil
    	package.loaded[mname]=nil

2. require ��ģ��

   - ��Ҫ����pcall, �����ڴ���ʱ, ֱ���˳�ִ����.
   - ����ʱ, ��Ҫ��oldmodule��ԭ����.
   - û�л�ԭ���ʲô������? (�µĵ��ò���? �ɵ���? Ϊ�ε�ʱ�ı�selfд����, �Ϳ���û��, ���������ȸ���? ��Ҳ�ȸ���������?)

   ```
   local bSucess,strErr = pcall( require, mname ) 
   if not bSucess then
   	_G[ mname ] = oldmodule
   	package.loaded[mname] = oldmodule
   	ScriptSys.log( 0 , "reload fail: " .. strErr )
   	return
   end
   ```

3.  �ȸ��߼�, ��������. Ҳ���ǻ�ԭ����. | ����  newmodule��, upvalue ����

   - ��������ֱ�ӻ�ԭ. 

     ```lua
     debug.setupvalue(env_f, i, old_value)
     ```


   - newmodule��upvalueΪtable, �滻oldmodule�е�table.  

     - ?


     - table�� pairs���Ա�����, key, val ȫ���滻Ϊold_value[key].
     - function ��Ҫ�ݹ鴦��upvalues.

   - new

4. ?

```lua
-- old_function key with new function_value
-- new funtion_value with old table_data
function hotfix(oldmodule, newmodule) -- �����ȸ�����
    local protection = {
        setmetatable = true,
        pairs = true,
        ipairs = true,
        next = true,
        require = true,
        _ENV = true,
    }
    --��ֹ�ظ���table�滻�������ѭ��
    local visited_sig = {}
    -- �����Ҫ����table�б���Ĳ���, �滻����table��
  	-- ʵ�ʸ��Ĳ���:1.������ַ; 2.key��ͬ, �����ͷ����仯��. 
    -- ע��,�ȸ�ʱ, newmoduleʹ�þɵ�table��ַ??
    -- �ڶ���Ҫע��, Ϊ��֧��table�ṹ�ȸ��޸�, ���ȿ�����ģ�������, ������ֶ�.
    -- �������,�������ж�̬��ֵ����, ������Ͳ�ͬ�ᱻ����(��ֵnil���Ḳ��). 
    -- ����һ������, ��ָ���������.
    function update_table(env_t, g_t, name, deep)
    	--��ĳЩ�ؼ����������бȶ�
        if protection[env_t] or protection[g_t] then return end
    	--���ԭֵ�뵱ǰֵ�ڴ�һ�£�ֵһ�������жԱ�. һ��table, �����Ƕ��������upvalue
        if env_t == g_t then return end
        local signature = tostring(g_t)..tostring(env_t)
        if visited_sig[signature] then return end
        visited_sig[signature] = true
    	--�����±� �Ա�ֵ������б���env���ƵĲ���
        for name, value in pairs(env_t) do
            local old_value = g_t[name]
            if type(value) == type(old_value) then --���Ͳ���
                if type(value) == 'function' then
                    update_func(value, old_value, name, deep..'  '..name..'  ')
          			-- function��ַ����, Ҫ���±������
                    g_t[name] = value 
                elseif type(value) == 'table' then
                    update_table(value, old_value, name, deep..'  '..name..'  ')
          			-- ע��, ����table��ַû��
                end
            else -- ���ͱ仯,ֱ�Ӹ���
                g_t[name] = value
            end
        end
    	--����table��Ԫ�����жԱ�
    	--��ʱ��Ҫ�䶯old_table��metatable? 
    	--�����Ǽ��ص�ʱ��,�ں�������ִ�е����, �޸���local table��metatable,
        --�����Ϊ������upvalue,�õ�����.
        local old_meta = debug.getmetatable(g_t)
        local new_meta = debug.getmetatable(env_t)
        if type(old_meta) == 'table' and type(new_meta) == 'table' then
            update_table(new_meta, old_meta, name..'s Meta', deep..'  '..name..'s Meta'..'  ' )
        end
    end
    -- ͨ��env��ǰ��ֵ��_G��ǰ����ֵ���жԱ�
    -- 1.������Ͳ�ͬ����ֱ�Ӹ���ԭֵ����ʱvalue��Ϊnil���������ԭֵ�����ǳ�nil�����
    -- 2.�����ǰֵΪ���������ǽ��к�����upvalueֵ�ȶ�
  	-- �ȸ���, ʹ�õ�function��newmodule_function, upvalue��_oldmodule_function�ĺ�����upvalue 
    -- ��,�µ��߼�, �µ�����(���й��������������)
    function update_func(env_f, g_f, name, deep) -- newmodule_function _oldmodule_function
        -- signature +visited_sig[] ����ݹ�
        local signature = tostring(env_f)..tostring(g_f)
        if visited_sig[signature] then return end
        visited_sig[signature] = true
        local old_upvalue_map = {}
        -- ��������_oldmodule_function��upvalueֵ
        for i = 1, math.huge do
            local name, value = debug.getupvalue(g_f, i)
            if not name then break end
            old_upvalue_map[name] = value
        end
    	--����_newmodule_function�д��ڵ�upvalue(��ʼֵ),
    	--ʹ��_oldmodule_function�е�ǰֵ, �滻�� _newmodule_function
        for i = 1, math.huge do
            local name, value = debug.getupvalue(env_f, i) 
            if not name then break end
            local old_value = old_upvalue_map[name]
            if old_value then-- �Ѿɵ�upvalueת�ƹ���
                if type(old_value) ~= type(value) then
                    debug.setupvalue(env_f, i, old_value) -- �� old_value �滻
                elseif type(old_value) == 'function' then
                    update_func(value, old_value, name, deep..'  '..name..'  ') -- �ݹ����
                elseif type(old_value) == 'table' then
          			-- oldtable�������, set to _newmodule_function
                    update_table(value, old_value, name, deep..'  '..name..'  ')
                    debug.setupvalue(env_f, i, old_value) 
                else -- ������ͨ����,ֱ��ʹ��old_value
                    debug.setupvalue(env_f, i, old_value)
                end
            end
        end
    end
  
    for name,value in pairs(newmodule) do
        local old_value = oldmodule[name]
        if type(old_value ) ~= type(value) then
      		-- ���ͱ������ֵΪ׼,��ֵ�ǿ�,�������ȸ���������ֵ��Ϊnil
            --_G[name] = value -- �����ƺ�Ӧ�ø�Ϊoldmodule
      		oldmodule[name] = value
        elseif type(value) == 'function' then
            update_func(value, old_value , name, 'oldmodule'..'  ')
   			-- ��߷�local ������ַ�����仯.(���溯����ַ��д��,��Ҫע��)
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

- ���ܻ��ɻ���Щnew��ȥ�Ķ�����ô��֤�õ��޸�?

  newObj ����ĳ�Աtable,����û�б䶯.

  newObj ͨ��Ԫ����÷���. ��oldmodule��ַû��, ������ĺ�����ַ�Ѿ�����

    `newObj.Foo` `newObj:Foo`�����ҵ��µĵ�ַ. 

  > ע��, �����������ַ�ʽ����, ���Ǳ���ķ�ʽ, ��ֻ�ܰ���ַ����.
  >
  > local lcFoo = newObj.Foo
  >
  > lcFoo() -- ����д��, �����ϰ汾�ĵ�ַ, ���ջ������ʽ�ĺ�����, �ȸ�����.

  ?

  ?


- ���ȸ�����������ɻ���, module�����local����

  local variable, local function �Ҷ��޷�ʹ�� pairs��������.  ��ʱ֪����function������up value �ĸ���.

  - ����ͨ��`debug.getupvalue` `debug.getlocal(f, i)`����ȡ������up value, ��local value
  - Ϊ���صز���module, ����, closure ����Lua�ʷ��������̵����֪ʶ. û���ҵ���.
  - [LUA�ⲿģ����ػ���](http://www.codedump.info/?p=402)
  - [�ʷ����� ���������](http://www.codedump.info/?p=310)
  - [Luaջ](http://www.codedump.info/?p=335)

  ### ʲô��function ��up value��?

  - [Lua�ȸ���(hotfix)](http://blog.csdn.net/jq0123/article/details/51819588)���������ҵ���

  - >���� test.lua:
    >local M = {}
    >local a = "old"
    >function M.get_a() return a end
    >return M
    >
    >���µ�
    >local M = {}
    >local a = "new"
    >function M.get_a() return a .. "_x" end
    >return M
    >
    >���� 
    >local hotfix = require("hotfix")
    >local test = hotfix.hotfix_module("test")
    >test.get_a()  -- "old_x"
    >
    >**���� a ��Ϊ������upvalue�õ��˱��������Ǻ����õ��˸���**��

    �ļ������������local a ��Ϊ������upvalue �õ�����. ������Ҷ���,��Ȼ���� �ؽ����ǰ��������.

    ?

### ��һ�� 

- ��Ҫ��lua 5.1��, ��� module�ͷ�module�������͵�,luaģ����ȸ�.

  - �ܷ�ʹ��require ֱ�Ӽ��ؽű���?

  - �Ƿ���Ҫ������д�����ļ���ʶ��?

  - ��Ŀ�е�reload, ��ģ��ļ���, ֱ�Ӹ�����local function ��local variable �����̻���δ��ȫ����. 

    ?

##### lua���ܺ�, �ȸ�ʧЧ�����

http://blog.csdn.net/x2345com/article/details/53510498

##### �ȸ�������

http://blog.csdn.net/qq_32319583/article/details/53223452

##### skynet����̨ʹ�ü��� 

http://blog.csdn.net/mycwq/article/details/50472692

http://blog.csdn.net/ad88282284/article/details/43994103

##### 5.2���ϵ������ݴ�

http://blog.csdn.net/cbbbc/article/details/45155135

