
NUM=4
collection = []

File.open(ARGV[0]).each_line { |l|
	collection << l.chomp!.split(',')
}

means = collection.inject([0] * NUM) { |a, b|
	a.each_index { |i|
		a[i] += b[i].to_i
	}
}.map { |v| 
	v  / collection.size
}

stdevs = collection.inject([0] * NUM) { |a, b|
	a.each_index { |i|
		a[i] += (b[i].to_i - means[i])
	}
}.map { |v| 
	Math.sqrt(v**2 / (collection.size - 1))
}

# Lambda Mean, Lambda Conf., Functor Mean, Functor Conf., Bind Mean, Bind Conf., Boost Mean, Boost Conf.
printf "%d,%f,%d,%f,%d,%f,%d,%f\n",
	means[0],
	stdevs[0],
	means[1],
	stdevs[1],
	means[2],
	stdevs[2],
	means[3],
	stdevs[3],
	means[4],
	stdevs[4]
