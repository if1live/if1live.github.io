fn calc_grade(score: i32) -> &'static str {
    match score {
        90...100 => "A",
        70...89 => "B",
        40...69 => "C",
        0...39 => "D",
        _ => "?",
    }
}

fn main() {
    let scores = vec![100, 90, 89, 70, 69, 40, 39, 0];
    for score in scores {
        let grade = calc_grade(score);
        println!("{} = {}", score, grade);
    }
}
