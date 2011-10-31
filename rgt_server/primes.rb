def find2km(n)
    k = 0
    q,r = n.divmod 2
    while r != 1
        n = q
        k = k.succ
        q, r = n.divmod 2
    end
    return k, n
end

# (eq. to) powMod m n k = n^k `mod` m
def powMod(m, n, k)
    x, y = n, 1
    loop do 
        q, r = k.divmod 2
        break if (k == 1)
        y = (x * y) % m if r != 0
        x = (x * x) % m
        k = q
    end
    return (x * y) % m
end

def test_primality(x)
    return x == 2 if x < 2 || x.even?
    rndg = Random.new
	
	r = Math.log2(x).ceil
    
    for i in 1..r
        do_next = false
        a = rndg.rand(2..(x-1))
        k, m = find2km(x - 1)
        x0 = powMod(x, a, m)
        next if x0 == 1 || x0 == (x-1)
        for j in 1...k
            x0 = (x0*x0).modulo x
            return false if x0 == 1
            if x0 == (x - 1)
                do_next = true
                break
            end
        end
        return false unless do_next
    end
    return true
end

