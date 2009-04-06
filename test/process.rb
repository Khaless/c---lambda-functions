
NUM=5
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

confIntervals = collection.inject([0] * NUM) { |a, b|
	a.each_index { |i|
		a[i] += (b[i].to_i - means[i])
	}
}.map { |v| 
	stdev = Math.sqrt(v**2 / (collection.size - 1.0))
	(1.96 * stdev) / Math.sqrt(collection.size)
}

# Lambda Mean, Lambda Conf., Functor Mean, Functor Conf., unary_func Mean. unaru_func Conf, Bind Mean, Bind Conf., Boost Mean, Boost Conf.
printf "%d %f %d %f %d %f %d %f %d %f\n",
	means[0],
	confIntervals[0],
	means[1],
	confIntervals[1],
	means[2],
	confIntervals[2],
	means[3],
	confIntervals[3],
	means[4],
	confIntervals[4],
	means[5],
	confIntervals[5]
