use std::char;
use std::collections::HashMap;
use std::env::args;
use std::error::Error;
use std::fs::File;
use std::hash::Hash;
use std::io::{BufRead, BufReader};

/* TODO
 * extract titles from gutenberg headers
 * multithread across multiple files
 * * may require contrival for practice
 * * time analysis?
 */

#[derive(Debug)]
struct Counthash<K: Hash + Eq + Clone>(HashMap<K, usize>);
impl<K: Hash + Eq + Clone> Counthash<K> {
    fn new() -> Self {
        Counthash(HashMap::new())
    }

    fn add(&mut self, key: &K) {
        let item = self.0.get_mut(&key);
        if let Some(count) = item {
            *count += 1;
        } else {
            self.0.insert(key.clone(), 1);
        }
    }

    fn sum(&self) -> usize {
        self.0.iter().map(|(_, v)| v).sum()
    }
}

#[derive(Debug)]
struct Stats {
    count: usize,
    mode: Option<(String, usize)>,
    longest: Option<String>,
    shortest: Option<String>,
}

impl Stats {
    fn new() -> Self {
        Stats {
            count: 0,
            mode: None,
            longest: None,
            shortest: None,
        }
    }
}

fn unmut<A: Clone, B: Clone>(opt: &Option<(&A, &B)>) -> Option<(A, B)> {
    opt.map(|(a, b)| (a.clone(), b.clone()))
}

fn analyze(source: BufReader<File>) -> Stats {
    // Should we just use the hash to derive other stats?
    let mut words: Counthash<String> = Counthash::new();
    let mut longest: Option<String> = None;
    let mut shortest: Option<String> = None;

    for line in source.lines().skip(19) {
        let uline = line.unwrap();

        if uline.contains("*** END OF THIS PROJECT GUTENBERG EBOOK") {
            break;
        }

        for word in uline.split(char::is_whitespace).filter(|w| *w != "") {
            words.add(&word.to_string());
            longest = longest
                .filter(|v| v.len() > word.len())
                .or(Some(word.to_string()));

            shortest = shortest
                .filter(|v| v.len() < word.len())
                .or(Some(word.to_string()));
        }
    }

    let mode = words.0.iter().fold(None, |acc, (w, c)| {
        acc.filter(|(_, ac)| *ac >= c).or(Some((w, c)))
    });

    Stats {
        count: words.sum(),
        mode: unmut(&mode),
        longest,
        shortest,
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let files: Vec<String> = args().skip(1).collect();
    let handler = File::open(files[0].clone())?;
    let reader = BufReader::new(handler);

    let stats = analyze(reader);

    println!(
        "count: {}\nmode: {:?}\nlongest: {:?}\nshortest: {:?}",
        stats.count, stats.mode, stats.longest, stats.shortest
    );

    Ok(())
}
