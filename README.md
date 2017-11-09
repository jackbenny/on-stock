`on-stock` is a small but useful inventory tool. As most of my small programs it
came to life from a real need of mine. There are many programs out there, big
programs, to keep track of your inventory. But all of these were to big and
complex for my own need. What I wanted was a small, preferably CLI-based,
inventory tool. So this program is the result of that. It may not meet the need
of other businesses, but for me it's more than enough.

# Usage

    Usage: ./on-stock [-l] [-s name][-m name 
    [-d name] [-n] [-h] [-f filename]
    
    -l      = list the articles in the database
    -s name = search for an 'name' in the database
    -m name = modify the article named 'name' in the database
         You'll then have the choice to change name, quantity and price.
    -d name = delete the article named 'name'
    -n      = create new articles (interactive mode only)
    -h      = display this help message

## Examples

	jake@red-dwarf:on-stock$ ./on-stock -n
	Could not open storage.bin
	Create the file and start adding records? (y/n): y
	Name ('done' when finished): Bolt
	Quantity: 89
	Price: 9.99
	Name ('done' when finished): Nail
	Quantity: 50
	Price: 0.50
	Name ('done' when finished): Hammer
	Quantity: 12
	Price: 59 
	Name ('done' when finished): done


	jake@red-dwarf:on-stock$ ./on-stock -l

    Name                            Quantity        Price
    =====================================================
    Bolt                            89              9.99
    Nail                            50              0.50
    Hammer                          12              59.00


    jake@red-dwarf:on-stock$ ./on-stock -m "Bolt"
    
    Name                            Quantity        Price
    =====================================================
    Bolt                            89              9.99
    
    What do you like to modify? (name, quantity, price): quantity
    Quantity (absolute value or a(dd)/s(subtract)NUMBER: s9

*Bolt* quantity is now 80.

    jake@red-dwarf:on-stock$ ./on-stock -d "Nail"
    
    Name                            Quantity        Price
    =====================================================
    Nail                            50              0.50
    
    Delete the record listed above? (y/n): y

*Nail* is now deleted from the database.

# License

`on-stock` is released under GNU GPLv2.
